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
		<a class="small" href="./sip.php">SIP</a>
		<a class="small" href="./cdp.php">CDP</a>
		<a class="small" href="http://katalyzer.sk/contact">Contact Us</a>
	</div></div>
</div>

<div id="top_container">
<div id="pannel_a">

	<div class="form2">
		Theme: <select id="theme_select" name="theme"></select>
	</div>
	Server: <select id="config_select"></select>

	<span id="nav-buttons">

	</span>


	<span id="format" class="group">Graph: <a href="?format=svg" id="format-svg" title="svg" class="button-selected">svg</a><a href="?format=png" id="format-png" title="png" class="button">png</a></span>
	<a href="graph.php" class="button">export</a>
</div>


<div id="table_loading_graph"><div class="graph_loading">Loading<br/><br/><img src="ajax/ajax-loader2.gif" /></div></div>
<div id="mouse_zoom" style="position:absolute;height:250px;width:870px;margin-top:10px;margin-left:60px;"></div>
<div id="mouse_text" style="position:absolute;display:none;background:cyan;opacity:0.4;color:black;padding:3px;width:200px;"></div>
<div id="log2">

</div>
<div id="pannel_b">
	<span class="group" id="shift">
		Shift: 
		<a href="?minus=100" class="button" title="minus=100">&lt;&lt;&lt;</a>
		<a href="?minus=25" class="button" title="minus=25">&lt;&lt;</a>
		<a href="?minus=10" class="button" title="minus=10">&lt;</a>
		<a href="?plus=10" class="button" title="plus=10">&gt;</a>
		<a href="?plus=25" class="button" title="plus=25">&gt;&gt;</a>
		<a href="?plus=100" class="button" title="plus=100">&gt;&gt;&gt;</a>
		<a href="?t=actual" class="button" title="t=actual">actual</a>
	</span>
	<span class="group" id="zoom">
		Time zoom:
		<a href="?zoom=0.25" class="button" title="zoom=0.25">+4x</a>
		<a href="?zoom=0.5" class="button" title="zoom=0.5">+2x</a>

		<a href="?zoom=2" class="button" title="zoom=2">-2x</a>
		<a href="?zoom=4" class="button" title="zoom=4">-4x</a>
	</span>
	<span id="yzoomm"  class="group">
		Scale:
		<a href="?yzoom=log" id="yzoom_log" title="log" class="button">log</a>
		<a href="?yzoom=100" id="yzoom_100" title="100" class="button-selected">lin 100%</a>
		<a href="?yzoom=25" id="yzoom_25" title="25" class="button">25%</a>
		<a href="?yzoom=5" id="yzoom_5" title="5" class="button">5%</a>
		<a href="?yzoom=1" id="yzoom_1" title="1" class="button">1%</a>
		<a href="?yzoom=0.1" id="yzoom_01" title="0.1" class="button">0.1%</a>
		<input id="yzoom_input" name="yzoom" type="text" size="3" value="100"/>%
		<a href="?yzoom=100" title="ok" class="button">ok</a>	</span>

</div>

</div>

<div class="left-panel">
	<div id="table_loading"></div>	<div id="log">
	<table id="iptable" cellspacing="0" summary="Zoznam ip + mac"  >
	<thead>
	<tr>
		<th scope="col" abbr="" class="left" >Loading ...</th>
	</tr>
	</thead>

	</table>
		<div id="pages">
			<div class="form3" id="custom_ip_form">
				Selected row: <input id="custom_ip" name="yzoom" type="text" size="15" value=""/>
				<a id="custom_ip_button" title="submit" class="button">ok</a>
			</div>
			<span id="page_switch">
			Page 1 of 1:
			</span>
		</div>
	</div>

</div>
<div class="right-panel">

	<form id="time_form">
		<h3>Time settings:</h3>

		<div id="tabs">
			<a href="#from" class="button">from</a>
			<a href="#to" class="button-selected">to</a>
			<a href="#from-to" class="button">range</a>
		</div>
		<div id="to" class="tabs_panel_active">
			<label>Date:<span class="long"><input id="ndate1" name="ndate1" type="text" value="07.11.2009" class="long" /></span></label>
			<label>Time:<span><input id="ntime1" name="ntime1" type="text" value="17:15" size="6" /></span></label>
			<label>Interval:<span>
				<select id="ninterval1" name="ninterval1" >
					<option  value ="custom">custom [min]</option>
					<option  value ="1">1 hour</option>
					<option  value ="2">3 hours</option>
					<option  value ="3">6 hours</option>
					<option  value ="4">12 hours</option>
					<option selected value ="5">1 day</option>
					<option  value ="6">1 week</option>
					<option  value ="7">1 month</option>
					<option  value ="8">1 year</option>
				</select>
			</span></label>
			<div class="r-form">
				<label><em class="invisible">_</em><span id="send-to"><a href="#" title="send" >Submit</a></span></label>
			</div>
			<div class="l-form">
				<label>Custom interval:<span><input id="ncustom1" name="ncustom1" type="text" value="1440" size="6" /></span></label>
			</div>
		</div>
		<div id="from" class="tabs_panel">
			<label>Date:<span class="long"><input id="ndate2" name="ndate2" type="text" value="06.11.2009" class="long" /></span></label>
			<label>Time:<span><input id="ntime2" name="ntime2" type="text" value="17:15" /></span></label>
			<label>Interval:<span>
				<select id="ninterval2" name="ninterval2">
					<option  value ="custom">custom [min]</option>
					<option  value ="1">1 hour</option>
					<option  value ="2">3 hours</option>
					<option  value ="3">6 hours</option>

					<option  value ="4">12 hours</option>
					<option selected value ="5">1 day</option>
					<option  value ="6">1 week</option>
					<option  value ="7">1 month</option>
					<option  value ="8">1 year</option>
				</select>
			</span></label>
			<div class="r-form">
				<label><em class="invisible">_</em><span id="send-from" ><a href="#" title="send">Submit</a></span></label>
			</div>
			<div class="l-form">
				<label>Custom interval:<span><input id="ncustom2" name="ncustom2" type="text" value="1440" /></span></label>
			</div>
		</div>		
		<div id="from-to" class="tabs_panel">
			<label>Begin date:<span class="long"><input id="ndate3b" name="ndate3b" type="text" value="06.11.2009" class="long" /></span></label>
			<label>Begin time:<span><input id="ntime3b" name="ntime3b" type="text" value="17:15" /></span></label>
			<label>End date:<span class="long"><input id="ndate3e" name="ndate3e" type="text" value="07.11.2009" class="long" /></span></label>
			<label>End time:<span><input id="ntime3e" name="ntime3e" type="text" value="17:15" /></span></label>
			<label><span id="send-from-to" ><a href="#" title="send">Submit</a></span></label>
		</div>
	</form>
	<div id="log3">
		
	</div>
<!--
	<a onclick="$('body').append('<div style=\'height:500px;\'></div>');" href="javascript:var firebug=document.createElement('script');firebug.setAttribute('src','http://getfirebug.com/releases/lite/1.2/firebug-lite-compressed.js');document.body.appendChild(firebug);(function(){if(window.firebug.version){firebug.init();}else{setTimeout(arguments.callee);}})();void(firebug);">Enable Firebug Lite</a>
-->
</div>

<hr>
</div>

</body></html>
