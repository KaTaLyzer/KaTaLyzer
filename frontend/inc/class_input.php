<?php

class input {

	// time
	public $begin;
	public $end;

	// table
	public $table_page;
	public $table_sort;
	public $table_sort_reverse;
	public $table_selected;
	public $table_selected_parent;

	// theme
	public $theme;
	public $theme_list;
	// config
	public $config;
	public $config_list;

	// graph
	public $graph_yzoom;
	public $graph_format;

	// measurement select boxs / buttons
	public $measure_sw; // array!
	public $measure_list;

	public $interval_human_readable;

	
	function __construct($config_dirs=array('configs'),$theme_dir='themes') {
		if(isset($_GET['reset_session'])){ unset($_SESSION['config'],$_SESSION['theme'],
				$_SESSION['table_page'],$_SESSION['table_sort'],$_SESSION['table_selected'],$_SESSION['table_sort_reverse'],
				$_SESSION['graph_format'],$_GET['graph_yzoom'],$_SESSION['end'],$_SESSION['begin'],$_SESSION['measure_sw']); }
		$this->configs($config_dirs);
		$this->themes($theme_dir);
		$this->graph_format();
		$this->graph_yzoom();
		$this->time();
		$this->table_page();
		$this->table_sort();
		$this->table_selected();
		$this->table_selected_parent();
		$this->generate_time_interval();
	}



	private function configs($config_dirs){
		if(!is_array($config_dirs)){
			$config_dirs = array($config_dirs);
		}
		$this->config_list = array();
		foreach($config_dirs as $dir){
			$this->config_list = array_merge($this->config_list, $this->dir_list($dir));
		}
		if(empty($this->config_list)){
			die('No configuration file found!');
		}

		if(isset($_GET['config']) && array_key_exists($_GET['config'],$this->config_list)){
			$this->config = $_GET['config'];
			$_SESSION['config'] = $this->config;
			$this->reset_table(); // on config change => reset all table settings
			unset($_SESSION['measure_sw']);
		}elseif(isset($_SESSION['config']) && array_key_exists($_SESSION['config'],$this->config_list)){
			$this->config = $_SESSION['config'];
		}else{
			$this->config = key($this->config_list);
		}
	}
	public function reset_config(){
		unset($_SESSION['config']);
		$this->config = key($this->config_list);
	}
	public function remove_bad_config($bad_key){
		if(isset($this->config_list[$bad_key])){
			unset($this->config_list[$bad_key]); 
		}
	}

	private function themes($theme_dir){
		$this->theme_list = $this->dir_list($theme_dir);
		if(isset($_GET['theme']) && array_key_exists($_GET['theme'],$this->theme_list)){
			$this->theme = $_GET['theme'];
			$_SESSION['theme'] = $this->theme;
		}elseif(isset($_SESSION['theme']) && array_key_exists($_SESSION['theme'],$this->theme_list)){
			$this->theme = $_SESSION['theme'];
		}else{
			$this->theme = key($this->theme_list);
			foreach($this->theme_list as $t => $v){
				if($t=='themes/black'){
					$this->theme = $t;
				}
			}
		}
	}

	private function dir_list($dir){
		$arr = array();
		if ($handle = opendir(getcwd()."/".$dir)) {
		    while (false !== ($file = readdir($handle))) {
			if (!preg_match("/^\./", $file)) {
				$arr[$dir.'/'.$file] = str_replace('.conf','',$file);
			}
		    }
		    closedir($handle);
		}
		return $arr;
	}


	private function graph_format(){
		if(isset($_GET['graph_format']) && ($_GET['graph_format']=='png' || $_GET['graph_format']=='svg') ){
			$this->graph_format = $_GET['graph_format'];
			$_SESSION['graph_format'] = $this->graph_format;
		}elseif(isset($_SESSION['graph_format'])){
			$this->graph_format = $_SESSION['graph_format'];
		}else{
			if(isset($_SERVER['HTTP_USER_AGENT']) && preg_match('/(opera|firefox|iceweasel|Safari)/i', strtolower($_SERVER['HTTP_USER_AGENT']) ) ){
				$this->graph_format = 'svg';
			}else{
				$this->graph_format = 'png';
			}
		}
	}

	private function graph_yzoom(){
		if(isset($_GET['graph_yzoom']) && (
				( is_numeric($_GET['graph_yzoom']) && $_GET['graph_yzoom']>=0.01 && $_GET['graph_yzoom'] <= 1000 )
				|| $_GET['graph_yzoom']=='log'
			)
		){
			$this->graph_yzoom = $_GET['graph_yzoom'];
			$_SESSION['graph_yzoom'] = $this->graph_yzoom;
		}elseif(isset($_SESSION['graph_yzoom'])){
			$this->graph_yzoom = $_SESSION['graph_yzoom'];
		}else{
			$this->graph_yzoom = 100;
		}
	}

	private function time(){
		if(isset($_SESSION['end'],$_SESSION['begin'])){
			$this->end = $_SESSION['end'];
			$this->begin = $_SESSION['begin'];
		}else{
			$this->end = time();
			$this->begin = $this->end - INTERVAL;
		}


		if(isset($_GET['t']) && $_GET['t']=='actual' ){
			// day = 86400
			$this->end = time();
			$this->begin = $this->end - INTERVAL;
		}elseif(isset($_GET['plus']) && is_numeric($_GET['plus']) ){
			$shift = ($this->end - $this->begin)*($_GET['plus']/100);
			$this->end += $shift;
			$this->begin += $shift;
		}elseif(isset($_GET['minus']) && is_numeric($_GET['minus']) ){
			$shift = ($this->end - $this->begin)*($_GET['minus']/100);
			$this->end -= $shift;
			$this->begin -= $shift;
		}elseif(isset($_GET['tab'],$_GET['ntime3b'],$_GET['ndate3b'],$_GET['ntime3e'],$_GET['ndate3e']) && $_GET['tab']==3){
			$begin = $this->date_convert($_GET['ntime3b'],$_GET['ndate3b']);
			$end = $this->date_convert($_GET['ntime3e'],$_GET['ndate3e']);
			if($end>$begin){
				$this->end = $end;
				$this->begin = $begin;
			}else{
				$this->end = $begin;
				$this->begin = $end;
			}
			if($this->end-$this->begin<300){
				$this->begin = $this->begin-round(150-(($this->end-$this->begin)/2));
				$this->end = $this->begin+300;
			}
		}elseif(isset($_GET['tab'],$_GET['ntime2'],$_GET['ndate2'],$_GET['ncustom2'],$_GET['ninterval2']) && is_numeric($_GET['ncustom2']) && $_GET['tab']==2){
			$this->begin = $this->date_convert($_GET['ntime2'],$_GET['ndate2']);
			if($_GET['ncustom2']>=5){
				$this->end = $this->begin + round($_GET['ncustom2']*60);
			}else{
				$this->end = $this->begin + 300;
			}
		}elseif(isset($_GET['tab'],$_GET['ntime1'],$_GET['ndate1'],$_GET['ncustom1'],$_GET['ninterval1']) && is_numeric($_GET['ncustom1']) && $_GET['tab']==1){
			$this->end = $this->date_convert($_GET['ntime1'],$_GET['ndate1']);
			if($_GET['ncustom1']>=5){
				$this->begin = $this->end - round($_GET['ncustom1']*60);
			}else{
				$this->begin = $this->end - 300;
			}
		}
		if(isset($_GET['zoom']) && is_numeric($_GET['zoom']) ){
                        $half_range = round(($this->end-$this->begin)/2);
                        $center = $this->begin + $half_range;
                        $this->end = $center + round($half_range*$_GET['zoom']);
                        $this->begin = $center - round($half_range*$_GET['zoom']);
                        // small
                        if($this->end-$this->begin<300){
                                $this->end = $center + 150;
                                $this->begin = $center - 150;
                        }elseif($this->end-$this->begin>126144000){
                                $this->end = $center + 63072000;
                                $this->begin = $center - 63072000;
                        }
                }

		$_SESSION['end'] = $this->end;
		$_SESSION['begin'] = $this->begin;
		

	}
	public function reset_time(){
		$this->end = time();
		$this->begin = $this->end - INTERVAL;
		$_SESSION['end'] = $this->end;
		$_SESSION['begin'] = $this->begin;
	}

	private function date_convert($stime,$sdate){
		if (isset($sdate)){
			if (isset($stime)){
			        $time_expl = explode(":", $stime);
				if(isset($time_expl[0]) && !isset($time_expl[1])){
					$time_expl[1] = 0;
					if($time_expl[0] < 0 || $time_expl[0] > 24)
						$time_expl[0] = 0;
				}
				elseif(isset($time_expl[0]) && isset($time_expl[1])){
					if($time_expl[0] < 0 || $time_expl[0] > 24)
						$time_expl[0] = 0;
					if($time_expl[1] < 0 || $time_expl[1] > 60)
						$time_expl[1] = 0;
				}else{
					$time_expl[0] = 0;
					$time_expl[1] = 0;
				}
			}else{
					$time_expl[0] = 0;
					$time_expl[1] = 0;
			}

			$date_expl = explode(".", $sdate);
			if(isset($date_expl[0]) && isset($date_expl[1]) && isset($date_expl[2])){
				if($date_expl[1] < 13 && $date_expl[1] > 0 && $date_expl[0] < 32 && $date_expl[0] > 0 && $date_expl[2] > 2000 && $date_expl[2] < 2100)
					$end = mktime($time_expl[0],$time_expl[1],0,$date_expl[1],$date_expl[0],$date_expl[2]);
				else
					$end = 0;
			}else{
				$end = 0;
			}
		}

		return $end;
	}

	public function process_config($tree,$level=0){
		if(!is_array($tree)){
			return false;
		}elseif(isset($_GET['measure_sw'][$level]) && array_key_exists($_GET['measure_sw'][$level],$tree)){
			$this->measure_sw[$level] = $_GET['measure_sw'][$level];
			$_SESSION['measure_sw'][$level] = $this->measure_sw[$level];
			if(isset($tree[$this->measure_sw[$level]]['tree'])){ // if exist subtree => reset table
				$this->reset_table();
			}
		}elseif(isset($_SESSION['measure_sw'][$level]) && array_key_exists($_SESSION['measure_sw'][$level],$tree)){
			$this->measure_sw[$level] = $_SESSION['measure_sw'][$level];
		}else{
			$this->measure_sw[$level] = key($tree);
		}
		//creating list
		foreach($tree as $k => $v){
			$this->measure_list[$level][$k]=$v;
			unset($this->measure_list[$level][$k]['tree']);
		}
		if(isset($tree[$this->measure_sw[$level]]['tree'])){ // if exist subtree => recursive!
			$this->process_config($tree[$this->measure_sw[$level]]['tree'],$level+1);
		}else{
			unset($this->measure_list[$level][key($this->measure_list[$level])]);
			if($this->measure_sw[$level]==0) // !!!! bad but working
				$this->measure_sw[$level]++;
		}
	}



	private function table_page(){
		if(isset($_GET['table_page']) &&  is_numeric($_GET['table_page'] ) && $_GET['table_page']>=0 ){
			$this->table_page = $_GET['table_page'];
			$_SESSION['table_page'] = $this->table_page;
		}elseif(isset($_SESSION['table_page'])){
			$this->table_page = $_SESSION['table_page'];
		}else{
			$this->table_page = 1;
		}
	}
	private function table_sort(){
		if(isset($_GET['table_sort']) &&  is_numeric($_GET['table_sort'] ) && $_GET['table_sort']>=0 ){
			$this->table_sort = $_GET['table_sort'];
			if(isset($_SESSION['table_sort']) && $this->table_sort == $_SESSION['table_sort']){  // second click => change orientation
				if(isset($_SESSION['table_sort_reverse'])){
					$this->table_sort_reverse = $_SESSION['table_sort_reverse'];
				}
				if($this->table_sort_reverse == 0){
					$this->table_sort_reverse = 1;
				}else{
					$this->table_sort_reverse = 0;
				}
			}else{
				$this->table_sort_reverse = 0;
			}
			$this->table_page = 1;
			$_SESSION['table_page'] = $this->table_page;
			$_SESSION['table_sort'] = $this->table_sort;
			$_SESSION['table_sort_reverse'] = $this->table_sort_reverse;
		}elseif(isset($_SESSION['table_sort'],$_SESSION['table_sort_reverse'])){
			$this->table_sort = $_SESSION['table_sort'];
			$this->table_sort_reverse = $_SESSION['table_sort_reverse'];
		}else{
			$this->table_sort = 1;
			$this->table_sort_reverse = 0;
		}
	}
	private function table_selected(){
		if(isset($_GET['table_selected']) ){
			$this->table_selected = $_GET['table_selected'];
			$_SESSION['table_selected'] = $this->table_selected;
		}elseif(isset($_SESSION['table_selected'])){
			$this->table_selected = $_SESSION['table_selected'];
		}else{
			$this->table_selected = 'all';
		}
	}
	private function table_selected_parent(){
		if(isset($_GET['table_selected_parent']) ){
			$this->table_selected_parent = $_GET['table_selected_parent'];
			$_SESSION['table_selected_parent'] = $this->table_selected_parent;
		}elseif(isset($_SESSION['table_selected_parent'])){
			$this->table_selected_parent = $_SESSION['table_selected_parent'];
		}else{
			$this->table_selected_parent = null;
		}
	}
	private function reset_table(){
		$this->table_page = 1;
		$this->table_sort = 1;
		$this->table_selected = 'all';
		$this->table_sort_reverse = 0;
		$_SESSION['table_page'] = $this->table_page;
		$_SESSION['table_sort'] = $this->table_sort;
		$_SESSION['table_selected'] = $this->table_selected;
		$_SESSION['table_sort_reverse'] = $this->table_sort_reverse;
	}


	private function generate_time_interval(){	// vypise merany interval

		$interval = $this->end - $this->begin;
		$interval = round($interval/60);	// minuty
		$return = '';
		if($interval < 60)
			$return.= $interval." min";
		elseif($interval < 60*24){
			$minutes = $interval%60;
			$hours = ($interval - $minutes)/60;
			if($minutes == 0)
				$return.= $hours." hour(s) ";
			else
				$return.= $hours." hour(s) ".$minutes." min ";
		}
		else{
			$hours = $interval % (60 * 24);
			$days = ($interval - $hours)/(60 * 24);

			if(round($hours/60) == 0)
				$return.= $days." day(s)";
			else
				$return.= $days." day(s) ".round($hours/60)." hour(s)";

		}
		$this->interval_human_readable = $return;
	}

}
