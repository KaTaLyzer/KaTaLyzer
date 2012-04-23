<?php

class db {	

	// input ----------------------------------------
	private $servername;
	private $username;
	private $password;
	private $database;
	private $table;
	private $table_suffix_last;
	private $table_relation_index_name;

	private $begintime;	// begin
	private $endtime;	// end

	// output ---------------------------------------
	private $tabledata = Array();
	/* tabledata format example:
		Array
		    (
			[0] => Array
			    (
				[192.168.1.1] => 123
				[192.168.1.2] => 643643
			    )
			[1] => Array
			    (
				[192.168.1.1] => 13421
				[192.168.1.2] => 145
			    )
		    )
	*/
	private $table_count;
	private $table_suffix;	// tablename suffix - generated automaticaly - convert() function

	private $time_interval;	// interval between records [seconds]
	private $no_data;
	private $db_loaded;
	private $result;	// data from db
	private $id2time;	// Example $id2time[30] = 123456789;
	private $result_fields;
	private $sd_table;
	private $sd_parent_address;
	public $page_records;
	/* Example
		Array
		(
		    [0] => stdClass Object
			(
			    [name] => id
			    [table] => traffic_1m
			    [def] => 
			    [max_length] => 2
			    [not_null] => 1
			    [primary_key] => 1
			    [multiple_key] => 0
			    [unique_key] => 0
			    [numeric] => 1
			    [blob] => 0
			    [type] => int
			    [unsigned] => 1
			    [zerofill] => 0
			)
		...
	*/
	public $debug;
	private $convert_selected_ip_or_mac = false;

	/*
	 *	Constructor
	 */
	function db(){
		$this->db_loaded = false;
		$this->no_data = false;
		$this->sd_table = false;
		// mysql connect
	//	mysql_connect($host,$username,$password);
	//	@mysql_select_db($database) or die( "Unable to select database");
	}

	// set server
	function set_server($servername){
		$this->servername = $servername;
	}
	// set username
	function set_username($username){
		$this->username = $username;
	}
	// set password
	function set_password($password){
		$this->password = $password;
	}
	// set database
	function set_database($database){
		$this->database = $database;
	}
	// set tablename
	function set_table($table,$table_suffix_last='',$table_relation_index_name='id'){
		$this->table = $table;
		$this->table_suffix_last = $table_suffix_last;
		$this->table_relation_index_name = $table_relation_index_name;
	}
	// set time
	function set_time($begin,$end){
		$this->begintime = $begin;
		$this->endtime = $end;
	}
	function set_sd_table($v){
		$this->sd_table = $v;
	}
	function set_sd_parent_address($v){
		$this->sd_parent_address = $v;
	}

	// private section ------------------

	private function convert(){
		if( $this->endtime - $this->begintime <= 24*60*60 ){
			$this->table_suffix = "1m";
			$this->time_interval = 60;	// 60 sec = 1 min
		}
		elseif( $this->endtime - $this->begintime <= 7*24*60*60 ){
			$this->table_suffix = "5m";
			$this->time_interval = 300;	// 300 sec = 5 min
			}
		elseif( $this->endtime - $this->begintime <= 31*24*60*60 ){
			$this->table_suffix = "30m";
			$this->time_interval = 1800;	 // 1800 sec = 30 min
		}
		elseif( $this->endtime - $this->begintime <= 365*24*60*60 ){
			$this->table_suffix = "2h";
			$this->time_interval = 7200;	 // 7200 sec = 2 hour
		}
		else{
			$this->table_suffix = "1d";
			$this->time_interval = 86400;	  // 86400 sec = 1 day
		}
	}
	private function load_db(){
		if(!$this->db_loaded)
			$this->reload_db();
	}
	private function reload_db(){
		$this->convert();
		// mysql connect
		@mysql_connect($this->servername,$this->username,$this->password) or die('Mysql connect failed! check your server, username and pass');
		@mysql_select_db($this->database) or die('Database not found');

		if($this->sd_table){ $tmp = explode('_',$this->table_relation_index_name); $this->table_relation_index_name = $tmp[0].'_SD_'.$tmp[1]; } // sdd _SD 

		// select time table
		$time_query = "SELECT time, ".$this->table_relation_index_name." AS id FROM ".$this->table."_".$this->table_suffix."_time WHERE time >= ".$this->begintime." && time <= ".( $this->endtime + $this->time_interval ).';';
		$this->debug['time_query'] = $time_query;
		$time_result = mysql_query($time_query);

		$time_id_begin = false;

		if($time_result!=false){
			while($row = mysql_fetch_object($time_result)){
				$this->id2time[$row->id] = $row->time;
				if($time_id_begin == false)
					$time_id_begin = $row->id;
				$time_id_end = $row->id - 1;
			}
			mysql_free_result($time_result);
		}

		if($time_id_begin==false){	// problem, we need some data for mysql_num_fields
			$time_result = mysql_query('SELECT MAX(time) as time, '.$this->table_relation_index_name.' AS id FROM '.$this->table."_".$this->table_suffix."_time group by time");
			if($time_result && $row = mysql_fetch_object($time_result)){
				$time_id_begin = $row->id;
				$time_id_end = $row->id;
			}else{
				$time_id_begin = 0;
				$time_id_end = 0;
			}
			$this->no_data = true;
		}

		// select data table
		$query = "SELECT * FROM ".$this->table."_".$this->table_suffix.$this->table_suffix_last." WHERE id >= $time_id_begin && id <= $time_id_end;";
		if($this->sd_table){
			if($this->table_suffix_last=='_IP') $ip_or_mac = 'IP';
			else $ip_or_mac = 'MAC';
			//if($ip_or_mac=='IP') { $this->sd_parent_address = ip2long($this->sd_parent_address); }
			if($ip_or_mac=='IP') { $this->sd_parent_address = sprintf("%u",ip2long($this->sd_parent_address)); }
			else { $this->sd_parent_address = hexdec(str_replace(':','',$this->sd_parent_address)); }
			$query = "SELECT id, ".$ip_or_mac."_1 as ".$ip_or_mac.", bytes_21 as bytes_S, packets_21 as packets_S, bytes_12 as bytes_D, packets_12 as packets_D FROM ".$this->table."_".$this->table_suffix.$this->table_suffix_last."_SD WHERE id >= $time_id_begin && id <= $time_id_end AND ".$ip_or_mac."_2 = ".mysql_real_escape_string($this->sd_parent_address)." ";
			$query .= "UNION SELECT id, ".$ip_or_mac."_2 as ".$ip_or_mac.", bytes_12 as bytes_S, packets_12 as packets_S, bytes_21 as bytes_D, packets_21 as packets_D FROM ".$this->table."_".$this->table_suffix.$this->table_suffix_last."_SD WHERE id >= $time_id_begin && id <= $time_id_end AND ".$ip_or_mac."_1 = ".mysql_real_escape_string($this->sd_parent_address).";";
		}
		$this->debug['data_query'] = $query;
		$this->result = mysql_query($query);

		/* get column metadata */
		$i = 0;
		$convert_ip = false; $convert_mac = false;
		while ($this->result != false && $i < mysql_num_fields($this->result)) {
			$meta = mysql_fetch_field($this->result, $i);
			if( ($meta->name=='IP') && $meta->numeric){
				$meta->numeric = false;
				$convert_ip = true;
				$this->convert_selected_ip_or_mac = 'ip';
			}
			if( ($meta->name=='MAC') && $meta->numeric){
                                $meta->numeric = false;
                                $convert_mac = true;
				$this->convert_selected_ip_or_mac = 'mac';
                        }
			$this->result_fields[$i] = $meta;
			if(!$meta->numeric) $string_position = $i;
			$i++;
		}
		//$tabledata 
		if(is_array($this->result_fields)){
			foreach($this->result_fields as $col => $meta){
				if($meta->numeric && $meta->name != 'id'){
					$column_arr[$col] = $meta->name;
				}
			}
			$column_arr_count = count($column_arr);
		}

		if($this->no_data==false){
			while($row = mysql_fetch_row($this->result)){
				$line = $row[$string_position];
				foreach ($column_arr as $col => $column){
						if(isset($this->tabledata[$column][$line])){
							$this->tabledata[$column][$line] += $row[$col];
						}
						else{
							$this->tabledata[$column][$line] = $row[$col];
						}
				}
			}
		}
		$this->db_loaded = true;
	}


	function ret_table($page=1,$page_length=25,$order=false,$reverse=false,$avg=false){
		$this->load_db();

		if($avg)
			$divisor = ($this->endtime - $this->begintime) / $this->time_interval; // BUG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		else
			$divisor = 1;

		$table['header']=$this->ret_columns();
		if($table['header']==false){
			$table['header'] = array('no data');
		}

		if(isset($table['header'][$order]))
			$order = $table['header'][$order];
		elseif(isset($table['header'][1]))
			$order = $table['header'][1];
		else
			$order = $table['header'][0];

		$table['sum'][0] = 'all';
		foreach($this->tabledata as $key => $col){
			if($avg)
				$sum = round(array_sum($col)/(count($this->tabledata[$table['header'][1]]) * $divisor),5);
			else
				$sum = array_sum($col);
			if (preg_match("/bytes/i",$key)){
				$table['sum'][] = $this->convert_bytes($sum);
			}else{
				$table['sum'][] = $sum;
			}
		}
		if(!isset($table['sum'][1]) && is_array($this->result_fields)){
			foreach($this->result_fields as $meta){
				if($meta->name != 'id' && $meta->numeric == true){
					$table['sum'][]='0';
				}
			}
		}
		$pager_counter = 0;
		if($this->no_data==false){

			if($order==$table['header'][0]){
				if($reverse)
					krsort($this->tabledata[$table['header'][1]]);
				else
					ksort($this->tabledata[$table['header'][1]]);
				$order = $table['header'][1];
			}else{
				if(!$reverse)
					arsort($this->tabledata[$order]);
				else
					asort($this->tabledata[$order]);
			}
			$c = 0;
			foreach($this->tabledata[$order] as $string => $value){
				if( $pager_counter>=($page-1)*$page_length && $pager_counter < $page*$page_length ){	// pager
					$table[$c][0] = $string;
					$j = 1;
					foreach($this->result_fields as $meta){
						if($meta->name=="IP" && is_numeric($table[$c][0])){
                                                	$table[$c][0] = long2ip($table[$c][0]);
                                        	}
						elseif($meta->name=="MAC" && is_numeric($table[$c][0])){
							$table[$c][0] = wordwrap(strtoupper(str_pad(dechex($table[$c][0]), 12, "0", STR_PAD_LEFT)), 2, ":", true); // convert longint to MAC yeaaaa 
						}
						if($meta->name != 'id' && $meta->numeric == true){
							$number = round($this->tabledata[$meta->name][$string]/$divisor,5);
							if (preg_match("/bytes/i",$meta->name)){
								$table[$c][$j] = $this->convert_bytes($number);
							}else{
								$table[$c][$j] = $number;
							}
							$j++;
						}
					}
					$c++;
				}
				$pager_counter++;
			}
		}
		$this->page_records = $pager_counter;
		return $table;
	}

	function convert_bytes($number){
		if($number > 1024*1024*1024){
			return round($number/(1024*1024*1024),1).'&nbsp;GiB';
		}elseif($number > 1024*1024){
			return round($number/(1024*1024),1).'&nbsp;MiB';
		}elseif($number > 1024){
			return round($number/1024,1).'&nbsp;kiB';
		}else{
			return $number.'&nbsp;B';
		}
	}

	function ret_columns(){
		$this->load_db();
		if(!is_array($this->result_fields)){
			return false;
		}
		foreach($this->result_fields as $meta){
			if($meta->numeric == false)
				$table[0] = $meta->name;
		}
		foreach($this->result_fields as $meta){
			if($meta->name != 'id' && $meta->numeric == true)
				$table[] = $meta->name;
		}
		return $table;
	}
	function ret_sum(){
		$this->load_db();
	//	print_r($this->tabledata);
	//	print_r($this->result_fields);

	}
	function ret_graph($column,$name,$avg=false){
		$this->load_db();

		// convert IP or MAC to integer
		if($this->convert_selected_ip_or_mac == 'ip' && $name!='all'){
			//$name = ip2long($name);
			$name = sprintf("%u",ip2long($name));
		}elseif($this->convert_selected_ip_or_mac == 'mac' && $name!='all'){
			//wordwrap(strtoupper(str_pad(dechex($table[$c][0]), 12, "0", STR_PAD_LEFT)), 2, ":", true);
			$name = hexdec(str_replace(':','',$name));
		}


		if($avg) $divisor = 60; else $divisor = 1;

		for($i = $this->begintime; $i <= $this->endtime; $i += $this->time_interval){
			$val = round($i/$this->time_interval);
			$graph[$val] = 0;
		}
		if($this->no_data==false){

			// find string column
			foreach($this->result_fields as $meta){
				if($meta->numeric == false)
					$string_column = $meta->name;
			}

			reset($this->id2time);
			$time = current($this->id2time);
			@mysql_data_seek($this->result,0);
			while($row = mysql_fetch_assoc($this->result)){
				if(isset($this->id2time[$row['id']]))
					$time = $this->id2time[$row['id']]/$this->time_interval;
				if($name == 'all' || $name == $row[$string_column]){
					if(isset($graph[$time])){
						$graph[$time] += $row[$column];
					}
				}
			}
		}
		foreach($graph as $key => $val){
			$graph2[$key*$this->time_interval] = $val/($this->time_interval/$divisor);
		}
		return $graph2;
	}

}

?>
