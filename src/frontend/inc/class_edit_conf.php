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
  public $commets;
  
  function conf_edit($name){
    $this->fr = $name;
    $this->list_interface = $this->interface_list("/sys/class/net/");
    array_push($this->list_interface, "auto");
  }
  
  function readf(){
    $count = 0;
    $configfile = file($this->fr, FILE_IGNORE_NEW_LINES|FILE_SKIP_EMPTY_LINES);
    foreach($configfile as $line) {
      if ($line) {
	if(preg_match("/^#/", $line)){
	  $this->commets[$count][] = $line;
	  continue;
	}
	$line = trim($line);
	$pieces = explode("=", $line);
	if(isset($pieces[0], $pieces[1])){
	  $option = strtoupper(trim($pieces[0]));
	  $value = trim($pieces[1]);
	  if($option == 'DB_HOST'){
	    $this->host = $value;
	    $count=2;
	    continue;
	  }
	  if($option == 'DB_USER'){
	    $this->user = $value;
	    $count=2;
	    continue;
	  }
	  if($option == 'DB_PASS'){
	    $this->pass = $value;
	    $count=2;
	    continue;
	  }
	  if($option == 'DB_NAME'){ 
	    $this->database = $value;
	    $count=2;
	    continue;
	  }
	  if($option == 'INTERFACE'){
	    $count=1;
	    $this->interface = $value;
	    continue;
	  }
	  $pieces2 = explode("_", $option);
	  if(isset($pieces2[0], $pieces2[1])){
	    $value = trim($pieces[1]);
	    if(($pieces2[0] == 'PROTOCOL')) {
	      $count=3;
	      $this->protocols[$pieces2[1]] = $value;
	      continue;
	    }
	    if ($pieces2[0] == 'TCP') {
	      $count=4;
	      $this->tcp[$value] = $value;
	      continue;
	    }
	    if ($pieces2[0] == 'UDP') {
	      $count=4;
	      $this->udp[$value] = $value;
	      continue;
	    }
	  }
	}
	$count=5;
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
   $f = fopen($this->fr,"w");
   foreach($new_line as $line) {
       fwrite($f, $line);
   }
   fclose($f);
   
   system("sudo cp ".$this->fr." /etc/katalyzer/config.conf");
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