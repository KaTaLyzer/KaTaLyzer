<?php

class ipinfo {
	// DB info
	private $servername;
	private $username;
	private $password;
	private $database;
	public $ip;

	public function __construct($servername,$username,$password,$database) {
		$this->servername = $servername;
		$this->username = $username;
		$this->password = $password;
		$this->database = $database;
		$this->connect();
	}


	public function find_mac($ip){
		$ip = ip2long($ip);
		$query = "SELECT mac FROM IPlist where ip = '".mysql_real_escape_string($ip)."'";
		$result=@mysql_query($query);
		if(!$result){
			return false;
		}elseif(!$data = mysql_fetch_object($result)){
			return false;
		}else{
			$mac = strtoupper(str_pad(dechex($data->mac), 12, "0", STR_PAD_LEFT));
			return wordwrap($mac, 2, ":", true);
		}
	}
	public function find_dns($ip){
		$query = "SELECT * FROM DNSlist where ip = '".mysql_real_escape_string($ip)."'";
		$result=@mysql_query($query);
		if($result && $data = mysql_fetch_object($result) ){
			if(strtotime($data->time) > time()-24*60*60){
				return $data->dns;
			}
		}
		$dns = gethostbyaddr($ip);
		$create_query = "CREATE TABLE IF NOT EXISTS `DNSlist` (
			  `ip` varchar(15) collate utf8_unicode_ci NOT NULL,
			  `dns` text collate utf8_unicode_ci NOT NULL,
			  `time` datetime NOT NULL,
			  PRIMARY KEY  (`ip`)
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;";
		mysql_query($create_query);
		$add_query = "
			INSERT INTO `DNSlist` (`ip`, `dns`, `time`) VALUES
			('".mysql_real_escape_string($ip)."', '".mysql_real_escape_string($dns)."', '".date('Y-m-d H:i:s')."')
			ON DUPLICATE KEY UPDATE `dns` = '".mysql_real_escape_string($dns)."', `time` = '".date('Y-m-d H:i:s')."';";
		mysql_query($add_query);
		return $dns;

	}

	private function connect(){
		@mysql_connect($this->servername,$this->username,$this->password) or die(json_encode(array('status'=>false,'message'=>'Cannot connect to mysql')));
		@mysql_select_db($this->database) or die(json_encode(array('status'=>false,'message'=>'Database not found!')));
	}
}
?>
