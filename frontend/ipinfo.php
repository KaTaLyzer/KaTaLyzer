<?php
error_reporting(0);
ini_set("display_errors", 0);

Header("Pragma: no-cache");
header('Cache-Control: no-cache, must-revalidate');
Header("Expires: ".GMDate("D, d M Y H:i:s")." GMT");
header('Content-type: application/json');

session_start();
define("INTERVAL", 86400);
define("PAGE",25);
require_once('inc/class_input.php');
require_once('inc/class_config.php');
require_once('inc/class_port_names.php');
require_once('inc/class_ipinfo.php');



if(isset($_GET['ip']) && preg_match("/^([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(\.([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])){3}$/", $_GET['ip'])){
	$json['status']=true;
	$ip = $_GET['ip'];
	$input = new input('configs');
	$config = new config($input->config);
	$input->process_config($config->tree);
	$ipinfo = new ipinfo($config->host,$config->user,$config->pass,$config->database);
	$json['mac'] = $ipinfo->find_mac($ip);
	$json['dns'] = $ipinfo->find_dns($ip);
	if(isset($_GET['whois'])){
		exec('whois '.$ip,$whois_output,$whois_status);
		if(!$whois_status){
			$json['data'] = $whois_output;
		}
	}
	if(isset($_GET['nslookup'])){
		exec('nslookup '.$ip,$nslookup_output,$nslookup_status);
		if(!$nslookup_status){
			$json['data'] = $nslookup_output;
		}
	}
	if(isset($_GET['geoip'])){
		if(function_exists('geoip_record_by_name')){
			$location = @geoip_record_by_name($ip);
			if(is_array($location)){
				foreach($location as $k => $v){
					$json['data'][] = ucfirst(str_replace('_',' ',$k)).':  '.$v;
				}
			}
			if(isset($location['latitude'],$location['longitude']) && is_numeric($location['latitude']) &&is_numeric($location['longitude']) ){
				$json['data'][] = '<br/><iframe width="724" scrolling="no" height="300" frameborder="1" src="http://maps.google.com/maps?q='.$location['latitude'].','.$location['longitude'].'&amp;t=h&amp;ie=UTF8&amp;z=10&amp;output=embed" marginwidth="0" marginheight="0"/>';
			}
		}else{
			$json['data'][] = 'Please install php5-geoip module and geoip database.';
		}

	}


	echo json_encode($json);
}else{
	echo json_encode(array('status'=>false));
}

?>
