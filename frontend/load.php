<?php
error_reporting(0);
ini_set("display_errors", 0);

Header("Pragma: no-cache");
header('Cache-Control: no-cache, must-revalidate');
Header("Expires: ".GMDate("D, d M Y H:i:s")." GMT");
header('Content-type: application/json');

require_once('inc/core.php');



// config section
$json['config_list'] = $input->config_list;
$json['config_selected'] = $input->config;

// theme section
$json['theme_list'] = $input->theme_list;
$json['theme_selected'] = $input->theme;

// graph section
$json['graph_format'] = $input->graph_format;
$json['graph_yzoom'] = $input->graph_yzoom;

// datetime section
$json['begin_date'] = date("d.m.Y",$input->begin);
$json['begin_time'] = date("H:i",$input->begin);
$json['end_date'] = date("d.m.Y",$input->end);
$json['end_time'] = date("H:i",$input->end);
$json['interval_min'] = round(($input->end - $input->begin)/60);
if($input->end-$input->begin==60*60){
	$json['interval'] = 1;
}elseif($input->end-$input->begin==180*60){
	$json['interval'] = 2;
}elseif($input->end-$input->begin==360*60){
	$json['interval'] = 3;
}elseif($input->end-$input->begin==720*60){
	$json['interval'] = 4;
}elseif($input->end-$input->begin==1440*60){
	$json['interval'] = 5;
}elseif($input->end-$input->begin==10080*60){
	$json['interval'] = 6;
}elseif($input->end-$input->begin==44640*60){
	$json['interval'] = 7;
}elseif($input->end-$input->begin==525600*60){
	$json['interval'] = 8;
}else{
	$json['interval'] = 'custom';
}
$json['begin_timestamp'] = $input->begin;
$json['end_timestamp'] = $input->end;
$json['graph_max_value'] = $svg->graph_max_value;
$json['graph_min_value'] = $svg->graph_min_value;
$json['graph_prefix'] = $svg->prefix;
$json['graph_unit'] = $svg->unit;
$json['graph_yzoom'] = $input->graph_yzoom;

if($input->table_selected_parent!=null){
	$json['table_selected_parent'] = $input->table_selected_parent;
}

// menu section
if(is_array($input->measure_list)){
	foreach($input->measure_list as $measure_key => $measure){
		unset($sel);
		if(count($measure)<=3){
			$sel['type'] = 'buttons_switch';
		}else{
			$sel['type'] = 'select';
		}
		$sel['id'] = 'measure_sw'.$measure_key;
		if(isset($measure[$input->measure_sw[$measure_key]]['select_name']))
			$sel['name'] = $measure[$input->measure_sw[$measure_key]]['select_name'];
		else
			$sel['name'] = '|';
		$sel['target'] = 'measure_sw['.$measure_key.']';
		$sel['selected'] = $input->measure_sw[$measure_key];
		foreach($measure as $key => $val){
			unset($a);
			if(isset($val['realname']))
				$a['name'] = $val['realname'];
			else
				$a['name'] = $val['name'];
			$a['val'] = $key;
			if(isset($val['group']) && $val['group']!=null)
				$a['group'] = $val['group'];
			$sel['data'][] = $a;
		}
		$json['menu'][] = $sel;
	}
}else{
	$json['menu'] = array('type'=>'buttons_switch','id'=>'measure_sw','name'=>'Error','target'=>'measure_sw','selected'=>'',array(array('name'=>'a','val'=>'a')));
}

$json['info_panel']['Time interval'] = array(
	'From:'=>date('d.m.Y H:i:s',$input->begin),
	'To:'=>date('d.m.Y H:i:s',$input->end),
	'Interval:'=>$input->interval_human_readable
);
$json['info_panel']['Table info'] = array(
	'Records:'=>$data->page_records,
);
if(isset($table['header'],$table['sum'])){
	foreach($table['header'] as $k => $v){
		if($k!=0 && isset($table['sum'][$k])){
			$json['info_panel']['Table info'][$v] = $table['sum'][$k];
		}
	}
}


// table section
$json['table'] = $table;
$c=-2;
$selected_row = null;
foreach($table as $v){  if($v[0]==$input->table_selected) $selected_row=$c; $c++; }
$json['table_selected_row'] = $selected_row;
$json['table_selected_row_val'] = $input->table_selected;
$json['table_sum'] = $data->page_records;
$json['table_page'] = $input->table_page;
$json['table_maxlenght'] = PAGE;



// graph section
if(!isset($_GET['table_page']) && !isset($_GET['table_sort']) ){
	if($input->graph_format == "svg")
		$json['graph'] = "<embed src=\"graph.php?id=".(rand()+time())."\" type=\"image/svg+xml\" width=940 height=300 >";
	else
		$json['graph'] = "<img src=\"graph.php?id=".(rand()+time())."\" width=\"940\" height=\"300\" />";
}

$debug['json generation'] = round(microtime(true) - $debug['svg generation'],6).' s';
$debug['runtime'] = round(microtime(true) - $debug['begin'],6).' s';
$debug['svg generation'] = round($debug['svg generation'] - $debug['database load'],6).' s';
$debug['database load'] = round($debug['database load'] - $debug['config load'],6).' s';
$debug['config load'] = round($debug['config load'] - $debug['begin'],6).' s';
$debug['begin'] = date('Y-m-d H:i:s',floor($debug['begin'])).'.'.str_replace('0.','',round($debug['begin']-floor($debug['begin']),3));
$unit=array('b','kb','mb','gb','tb','pb');
$debug['memory get peak usage'] = round(memory_get_peak_usage(true)/pow(1024,($i=floor(log(memory_get_peak_usage(true),1024)))),2).' '.$unit[$i];
$json['debug'] = $debug;
$json['debug_mysql'] = $data->debug;

echo json_encode($json);

?>
