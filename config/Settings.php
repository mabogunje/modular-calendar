<?php // Calendar Settings ?>

<?php
/**
* Settings Class: Contains all default calendar settings
* property@ cell_body: Cell content style class
* property@ cell_head: Cell header style class
* property@ cell_class: Cell style class
* property@ cell_height: Cell height
* property@ cell_width: Cell width
* property@ cell_colspan: Cell column span
* property@ cell_rowspan: Cell row span
* property@ row_class: Row style class
* property@ row_height: Row height value
* property@ table_height: Table height value
* property@ table_width: Table width value
* property@ table_cell_padding: Cell padding value
* property@ table_cell_spacing: Cell spacing value
* property@ table_border: Table border width
* property@ table_class: Style Class for table
* property@ table_title_class: Style Class for table header
* property@ controls_class: Style class for Calendar buttons container
* property@ control_class: Style class for each calendar button
* property@ control_selected: Style class for selected control
* property@ event_page: Url to go to on event click
* property@ max_events: Maximum number of events to display in list views of calendar
*/

class Settings
{ 
	/* Cell Settings */
	public static $cell_body = "cbody" ;
	public static $cell_head = "chead" ;
	public static $cell_class = "cell" ;
	public static $cell_height = "#" ;
	public static $cell_width = "10%" ;
	public static $cell_colspan = 1 ;
	public static $cell_rowspan = 1 ;
	
	/* Row Settings */
	public static $row_class = "row" ;
	public static $row_height = "#" ;
	
	/* Table Settings */
	public static $table_height = 400 ;
	public static $table_width = "100%" ;
	public static $table_cell_padding = 5 ;
	public static $table_cell_spacing = 2 ;
	public static $table_border = 1 ;
	public static $table_class = "calendar" ;
	public static $table_title_class = "title" ;
	
	/* Control Settings */
	public static $controls_class = "controls" ;
	public static $control_class = "control" ;
	public static $control_selected = "sControl" ;
	
	/* General Settings */
	public static $selected = "highlight" ;
	public static $event_page ;
	public static $max_events = 4 ; 
	
	/**
	* cur_page() Method(): Returns the relative url of the current page (minus query) on which calendar is instantiated
	* return this_uri
	*/
	
	public function cur_page()
	{
		$Curr_uri = $_SERVER["REQUEST_URI"] ;
		
		if( strpos($Curr_uri, "?") !== false )
			$this_uri = substr($Curr_uri, 0, strpos($Curr_uri, "?") ) ; // Strip arguments

		return $this_uri ; 
	}
	
}

/**
* Cal_Database Class: Stores all information for accessing and Cal_Database
* property@ host: Cal_Database host
* property@ user: Cal_Database user login id
* property@ password: Cal_Database user password
* property@ name: Cal_Database name
* property@ tables: Associative array of table content->Cal_Database table name
*/

class Cal_Database extends Settings
{
	public static $host     = "hostname" ;
	public static $user     = "dbUser" ;
	public static $password = "dbPassword" ;
	public static $name     = "database" ;
	public static $tables   = array( "events"=>'events', "calendar"=>'calendar' ) ;
	
	/**
	* connect() Method: Establishes connection with Cal_Database
	* return void
	*/
	
	public static function connect()
 	{
		echo $user ;
 		$con = mysql_connect(Cal_Database::$host, Cal_Database::$user, Cal_Database::$password) ;
		if(!$con)
			die("Could not connect: " . mysql_error() . " :(") ;
		else
			mysql_select_db(Cal_Database::$name, $con) ;
 	}
}

?>
