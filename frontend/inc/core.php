<?php
$debug['begin'] = microtime(true);
session_start();
define("INTERVAL", 86400);
define("PAGE",25);
require_once('inc/class_input.php');
require_once('class_config.php');
require_once('class_port_names.php');
require_once('class_db.php');
require_once('class_svg.php');

$input = new input('configs');
$config = new config($input->config);
$input->process_config($config->tree);

////////////////////
//$input->sd_table = true;

require_once($input->theme.'/colors.php');

$debug['config load'] = microtime(true);

$data = new db();
$data->set_server($config->host);
$data->set_username($config->user);
$data->set_password($config->pass);
$data->set_database($config->database);

//$input->table_selected_parent='192.168.1.1';
if($input->table_selected_parent!=null) $data->set_sd_table(true);
$data->set_sd_parent_address($input->table_selected_parent);
$tree_depth = count($input->measure_list);
$table_prefix = $input->measure_list[0][$input->measure_sw[0]]['dbname'];
$table_keyname = $input->measure_list[$tree_depth-2][$input->measure_sw[$tree_depth-2]]['keyname'];

$table_suffix = '';
foreach($input->measure_list as $measure_list_key => $measure_list_value){
	if($measure_list_key!=0 && $measure_list_key!= ($tree_depth-1) ){ // disable first and last
		$table_suffix .= $measure_list_value[$input->measure_sw[$measure_list_key]]['dbname'];
	}
}
$name = $input->measure_list[0][$input->measure_sw[0]]['name'];
$units = $input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['unit'];
if(isset($input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['unit_divisor'])){
	$unit_divisor = $input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['unit_divisor'];
}else{
	$unit_divisor = null;
}

$data->set_table($table_prefix,$table_suffix,$table_keyname);
$data->set_time($input->begin,$input->end);

if(isset($input->measure_list[0][$input->measure_sw[0]]['settings']['avg'])){
	$avg = $input->measure_list[0][$input->measure_sw[0]]['settings']['avg'];
}else{
	$avg = false;
}
$graph_name = $input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['name'];

$graph = $data->ret_graph($graph_name,$input->table_selected,$avg);
if(isset($input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['second_name'])){
	$second_graph = $data->ret_graph($input->measure_list[$tree_depth-1][$input->measure_sw[$tree_depth-1]]['second_name'],$input->table_selected,$avg);
}
$table = $data->ret_table($input->table_page,PAGE,$input->table_sort,$input->table_sort_reverse,$avg);

$debug['database load'] = microtime(true);

$svg = new svg();
$svg->name($name);
$svg->unit($units);
$colors = Array('blue','green','red','cyan','magneta','yellow');

if(isset($second_graph)){
	$svg->insert($second_graph,'second');
	$svg->graph_color($graph_theme['color'][1],'second');
}
$svg->insert($graph,'main');
$svg->graph_color($graph_theme['color'][0],'main');
if($input->graph_yzoom == "log"){
	$svg->logarithmic();
}else{
	$svg->top_zoom($input->graph_yzoom/100);
}
$svg->grid_thickness(0.3);
$svg->grid_color('black');
$svg->frame_color('black');
$svg->text_color('black');
$svg->background_color($graph_theme['background']);

if($input->graph_format == "svg"){
	$_SESSION['image'] = $svg->ret();
	$_SESSION['image_header'] = 'Content-type: image/svg+xml';
}
elseif($input->graph_format == "png"){
	file_put_contents('/tmp/katalyzer.svg', $svg->ret() );
	exec("convert /tmp/katalyzer.svg /tmp/katalyzer.png");
	$_SESSION['image'] = file_get_contents('/tmp/katalyzer.png');
	$_SESSION['image_header'] = 'Content-type: image/png';
}
$debug['svg generation'] = microtime(true);

if(isset($_GET['reset_session'])){ unset($_SESSION['config'],$_SESSION['theme'],$_SESSION['table_page'],$_SESSION['table_sort'],$_SESSION['table_selected'],$_SESSION['table_sort_reverse']); }

?>
