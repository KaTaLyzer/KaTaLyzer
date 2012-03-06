<?php
class conf_edit {
  
  public $fr;
  public $host;
  public $user;
  public $pass;
  public $database;
  public $interface;
  public $protocols;
  public $tcp;
  public $udp;
  public $list_interface;
  
  function conf_edit($name){
    $this->fr = $name;
    $this->list_interface = $this->interface_list("/sys/class/net/");
    array_push($this->list_interface, "auto");
  }
  
  function readf(){
    $configfile = file($this->fr, FILE_IGNORE_NEW_LINES|FILE_SKIP_EMPTY_LINES);
    foreach($configfile as $line) {
      if ($line && !preg_match("/^#/", $line)) {
	$line = trim($line);
	$pieces = explode("=", $line);
	if(isset($pieces[0], $pieces[1])){
	  $option = strtoupper(trim($pieces[0]));
	  $value = trim($pieces[1]);
	  if($option == 'DB_HOST'){
	    $this->host = $value;
	    continue;
	  }
	  if($option == 'DB_USER'){
	    $this->user = $value;
	    continue;
	  }
	  if($option == 'DB_PASS'){
	    $this->pass = $value;
	    continue;
	  }
	  if($option == 'DB_NAME'){ 
	    $this->database = $value;
	    continue;
	  }
	  if($option == 'INTERFACE'){
	    $this->interface = $value;
	    continue;
	  }
	  $pieces2 = explode("_", $option);
	  if(isset($pieces2[0], $pieces2[1])){
	    $value = trim($pieces[1]);
	    if(($pieces2[0] == 'PROTOCOL')) {
	      $this->protocols[$pieces2[1]] = $value;
	      continue;
	    }
	    if ($pieces2[0] == 'TCP') {
	      $this->tcp[$value] = $value;
	      continue;
	    }
	    if ($pieces2[0] == 'UDP') {
	      $this->udp[$value] = $value;
	      continue;
	    }
	  }
	}
      }
    }
  }
  
  function writef($value){
    $find_tcp=false;
    $find_udp=false;
    
    $configfile = file($this->fr);
    foreach($configfile as $line) {
      if(!empty($line) && !preg_match("/^#/", $line)){
	$line = trim($line);
	$pieces = explode("=", $line);
	if (isset($pieces[0])) {
	  $option = strtoupper(trim($pieces[0]));
	  if (($option == 'DB_HOST') OR ($option == 'DB_USER') OR ($option == 'DB_PASS') OR ($option == 'INTERFACE') OR ($option == 'DB_NAME')) {
	    $new_line[]= $option."=".$value[$option]."\n";
	    continue;
	  }
	  $pieces2 = explode("_", $option);
	  if (isset($pieces2[0], $pieces2[1])) {
	      if ($pieces2[0] == 'PROTOCOL') {
		$prot=$value['protocols'];
		$key = array_search($pieces2[1], $prot);
	          if ($prot[$key] == $pieces2[1]) {
	              $new_line[]="PROTOCOL_".$pieces2[1]."=1\n";
		  }
		  else {
	              $new_line[]="PROTOCOL_".$pieces2[1]."=0\n";
		  }
		  continue;
	      }
	      if ($pieces2[0] == 'TCP')  {
	          if (!$find_tcp) {
	              $find_tcp=true;
		      $tcpp = explode(";", $value['tcp_port']);
		      foreach($tcpp as $tcp_port) {
			if(!empty($tcp_port))
		          $new_line[]="TCP_PORT=".$tcp_port."\n";
		      }
	          }
	          continue;
	      }
	      if ($pieces2[0] == 'UDP') {
	          if (!$find_udp) {
	              $find_udp=true;
		      $udpp = explode(";", $value['udp_port']);
		      foreach($udpp as $udp_port) {
			if(!empty($udp_port))
		          $new_line[]="UDP_PORT=".$udp_port."\n";
		      }
	          }
	          continue;
	      }
	  }
	}
      }
      else {
	$new_line[]=$line;
      }
    }
   
   unlink($this->fr);
   $f = fopen($this->fr,"at");
   foreach($new_line as $line) {
       fwrite($f, $line);
   }
   fclose($f);
  }
  
  function interface_list($dir){
    $arr = array();
    if ($handle = opendir($dir)) {
      while (false !== ($file = readdir($handle))) {
	if (!preg_match("/^\./", $file)) {
	  $arr[] = $file;
	}
      }
      closedir($handle);
    }
    return $arr;
  }
}

?>