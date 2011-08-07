<?php
error_reporting(E_ALL); 
ini_set("display_errors", 1); 
// headers 
Header("Pragma: no-cache");
Header("Cache-Control: no-cache");
Header("Expires: ".GMDate("D, d M Y H:i:s")." GMT");

require_once('inc/core.php');


?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>
<script type="text/javascript" src="ajax/jquery-1.3.2.min.js"></script>
<script type="text/javascript" src="ajax/jquery-ui-1.7.2.custom.min.js"></script>
<script type="text/javascript" src="ajax/global.js"></script>

<link rel="stylesheet" type="text/css" href="<?php echo $input->theme; ?>/style.css" media="screen" />
<link rel="stylesheet" type="text/css" href="<?php echo $input->theme; ?>/jquery-ui/jquery-ui-1.7.2.custom.css" media="screen" />

</head>
<body>
<div class="container">
<div id="top_menu">

	<div id="nadpis">
		<h1><a href=".">KaTaLyzer</a></h1>
	</div>
	<div id="navlist"><div id="navlist_left">
		<a href="http://katalyzer.sk/">Home</a>

		<a class="small" href="./index.php">Back</a>
	</div></div>
</div>

<div id="log2">

</div>

<div class="left-panel">
<?php
mysql_connect($config->host,$config->user,$config->pass);
@mysql_select_db($config->database) or die('Database not found');
#}else{
#	$time_id_begin = 0;
#	$time_id_end = 0;
#}

$time_result = mysql_query('SELECT time as time, MIN(id) AS id FROM CDP_1mm_time');
if($time_result && $row = mysql_fetch_object($time_result)){
	$time_id_begin = $row->id;
}else{
	$time_id_begin = 0;
}

$time_result = mysql_query('SELECT time as time, MAX(id) AS id FROM CDP_1mm_time');
if($time_result && $row = mysql_fetch_object($time_result)){
	$time_id_end = $row->id;
}else{
	$time_id_end = 0;
}


#$queryc="SELECT c.ttl, c.device_ID, c.Capabilities, c.version, c.platform, a.IP FROM CDP_1m c, CDP_ADDRS_1m a WHERE c.id >= $time_id_begin && c.id <= $time_id_end and a.id=c.id ;";
$queryc="SELECT * FROM (SELECT t.time, c.ttl, c.device_ID, c.Capabilities, c.version, c.platform, a.IP FROM CDP_1mm c, CDP_ADDRS_1m a, CDP_1mm_time t  WHERE c.id >= $time_id_begin && c.id <= $time_id_end and a.id=c.id and t.id=c.id ORDER BY t.time DESC)t GROUP BY ttl, device_ID, Capabilities, version, platform, IP ;";
$result=mysql_query($queryc);
if(!$result){
	echo "Error: ". mysql_error();
}

echo "<table>";
echo "<tr>";
echo "<td>Time</td><td>TTL</td><td>ID zariadenia</td><td>Vybavenie</td><td>Verzia zariadenia</td><td>Platforma zariadenia</td><td>IP adresy  zariadenia</td>";
echo "</tr>";
while($row=mysql_fetch_row($result)){
	echo "<tr>";
	for($j=0; $j<count($row); $j++) {
		if($j==0){
		echo "<td>".date("d.m H:i", $row[$j])."  </td>";
		}
		else{
			if($j>5)
				echo "<td>".long2ip($row[$j])."</td>";		
			else
				echo "<td>".$row[$j]."</td>";
		}
	}
	echo "</tr>";
}

echo "</table>";

?>

</div>
<div class="right-panel">
	<div id="log3">

	</div>
	<a onclick="$('body').append('<div style=\'height:500px;\'></div>');" href="javascript:var firebug=document.createElement('script');firebug.setAttribute('src','http://getfirebug.com/releases/lite/1.2/firebug-lite-compressed.js');document.body.appendChild(firebug);(function(){if(window.firebug.version){firebug.init();}else{setTimeout(arguments.callee);}})();void(firebug);">Enable Firebug Lite</a>
</div>

<hr>
</div>

</body></html>
