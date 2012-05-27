<?php
session_start();
include('inc/class_sip_svg.php');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head><link rel="stylesheet" type="text/css" href="themes/black/style.css" media="screen" /></head>
<body>
<div class="container">
	<div id="top_menu">
		<div id="nadpis">
			<h1><a href=".">KaTaLyzer</a></h1>
		</div>
		<div id="navlist"><div id="navlist_left">
			<a href="./index.php">Home</a>
			<a class="small" href="sip.php">SIP</a>
		</div></div>
	</div>

<?php
$selected_session = 1;
if(isset($_GET['id']) && is_numeric($_GET['id'])) $selected_session = intval($_GET['id']);

echo ("<div id='log2' style='height:auto;'><embed src='graph.php?id=".(rand()+time())."' type='image/svg+xml' width=940></div>");

echo ("<br/><table id='iptable' cellspacing='0' summary='Zoznam ip + mac'><thead><tr><th class='left'>Call ID</th><th>Source IP</th><th>destination IP</th></tr></thead><tbody>");


$conn = mysql_connect('localhost', 'root', 'root');
mysql_select_db("TRAFFIC_MONITORING");
$query=mysql_query('SELECT * FROM SIP_1m');

$while_count=0;
while($row=mysql_fetch_array($query)){
	$ip1 = $row['ip_s0'].'.'.$row['ip_s1'].'.'.$row['ip_s2'].'.'.$row['ip_s3'];
	$ip2 = $row['ip_d0'].'.'.$row['ip_d1'].'.'.$row['ip_d2'].'.'.$row['ip_d3'];
	//zobrazenie grafu
	if($row['id']==$selected_session){
		$ex = explode(" ", $row['postup']);
		$i = 0;
		foreach($ex as $v){ 
			$i++;
			if($i==1){
				if($v == 'a') $d['d'] = true;
				else $d['d'] = false;
			}
			elseif($i==2)
				$d['t'] = date('Y-m-d h:i:s',$v);
			elseif($i==3){
				if($v==0) $d['x'] = 'UNKNOWN';
				elseif($v==1) $d['x'] = 'REQUEST';
				elseif($v==2) $d['x'] = 'RESPOND';
			
				$d['x'] = $d['t'].' - '.$d['x'];
			}
			elseif($i==4){
				if($v==0) $comm = '';
				elseif($v==0) $comm = 'UNKNOWN';
				elseif($v==1) $comm = 'ACK';
				elseif($v==2) $comm = 'BYE';
				elseif($v==3) $comm = 'CANCEL';
				elseif($v==4) $comm = 'INFO';
				elseif($v==5) $comm = 'INVITE';
				elseif($v==6) $comm = 'MESSAGE';
				elseif($v==7) $comm = 'NOTIFY';
				elseif($v==8) $comm = 'OPTIONS';
				elseif($v==9) $comm = 'PRACK';
				elseif($v==10) $comm = 'PUBLISH';
				elseif($v==11) $comm = 'REFER';
				elseif($v==12) $comm = 'REGISTER';
				elseif($v==13) $comm = 'SUBSCRIBE';
				elseif($v==14) $comm = 'UPDATE';

				elseif($v>=100 && $v<=199) $comm = 'Informational';
				elseif($v>=200 && $v<=299) $comm = 'Successful';
				elseif($v>=300 && $v<=399) $comm = 'Redirection';
				elseif($v>=400 && $v<=499) $comm = 'Client failure';
				elseif($v>=500 && $v<=599) $comm = 'Server failur';
				elseif($v>=600 && $v<=699) $comm = 'Global failure';

				$d['x'] .= ' '.$comm;
				$i=0;
				$data[]=$d;
			}
		}
		$svg = new sip_svg();
		$svg->data = $data;
		$svg->s_ip = $ip1;
		$svg->d_ip = $ip2;
		$_SESSION['image'] = $svg->ret();
		$_SESSION['image_header'] = 'Content-type: image/svg+xml';
	}
	$while_count++;
	//zobrazenie tabulky
	echo '
	<tr '.($while_count%2==1?'class="even"':'').' onclick="window.location = \'?id='.$row['id'].'\'">
		<th class="spec'.($while_count%2==1?'alt':'').'">'.str_replace('Call-ID: ','',$row['callid']).'</td>
		<td>'.$ip1.'</td>
		<td>'.$ip2.'</td>
	</tr>';
}
?>
</tbody></table>
</div>
</body></html>
