<?php // These are the management classes for managing the calendar ?>
<?php require_once("cal-DBAL.inc") ; ?>

<?php
/**
* iCalManage Interface: Describes the basic interaction of all calendar management classes
* exists($obj) Method: Check if obj exists in database
* roster() Method: Returns an array of objects for all entries in corresponding database table
* save($obj) Method: Inserts/Updates the object obj in the database
* delete($obj) Method: Removes the object from the database
* refresh($obj) Method: Refreshes the data of the object by retreiving values from database
* search($value) Method: Searches the corresponding database for an with field value matching 'value' 
*/

interface iCalManage
{
	function exists($obj) ;
	function roster() ;
	function save($obj) ;
	function delete($obj) ;
	function refresh($obj) ;
	//function search($value) ; // To Do
}

/**
* Cal_Management Class: This class provides static access to the Manager object
* property@ Manage: See Manager class 
*/

class Cal_Management
{	
	public static $Manage ;

	/**
	* setup() Method: Assigns $Manage to Manager object. Reason: Static cannot be assigned to obj before runtime
	* return void
	*/
	
	static function setup()
	{
		self::$Manage = new CalManager() ;
	}	
}

/**
* CalManager Class: This is the wrapper for all calendar manager classes; All management is accessed through this class
* property@ Events: Provides Access to the EventManager for handling Database events
* property@ Scheduler: Provides Access to the ScheduleManager for handling Database date->event relationships
* Search(value) Method: Searches all tables for value 'value' -- To Do
*/

class CalManager
{
	function __construct()
	{
		$this->Events = new EventManager() ;
		$this->Agenda = new ScheduleManager() ;
	}	
}

/**
* EventManager Class: Manages Events in the database
*/

class EventManager implements iCalManage
{	
	private $query ;
	
	function exists($obj)
	{
		$find = "SELECT * FROM %s WHERE  id = '%s'" ;
		$this->query = sprintf($find, mysql_real_escape_string(Cal_Database::$tables[events]),
									  mysql_real_escape_string($obj->get_id())
					    );
		$result = mysql_query($this->query) ;
		
		if(!$result)
		{
			$msg = 'Invalid query: ' . mysql_error() . "<br>" ;
			$msg .= 'Whole query: ' . $this->query ;
			die($msg) ;
		}
		
		if( $result = mysql_fetch_assoc($result) )
		{
			$obj->set_id( $result[id] ) ;
			return true ;
		}
		else
			return false ;			
	}

	function roster()
	{
		$roster = 'SELECT * FROM %s' ;
		$eventList = array() ;
		
		$this->query = sprintf($roster, Cal_Database::$tables[events]) ;
		
		if( $result = mysql_query($this->query) )
		{
			while($row = mysql_fetch_assoc($result))
			{
				$event = new Event() ;
				$event->set_id($row[id]) ;				
				
				$this->refresh($event) ;
				$eventList[] = $event ;
			}
		}
		
		return $eventList ;
	}
		
	function save($obj)
	{
		$insert = 'INSERT INTO %s VALUES(NULL, "%s", "%s", "%s", "%s", "%s", "%s", "%s")' ;
		$update = 'UPDATE %s SET name="%s", type="%s", feature="%s", location="%s", description="%s", fee="%s", url="%s"
				   WHERE id="%s"' ;
		
		if( !$this->exists($obj) )
		{
			$this->query = sprintf($insert, mysql_real_escape_string(Cal_Database::$tables[events]),
									mysql_real_escape_string($obj->name), 
									mysql_real_escape_string($obj->type),
									mysql_real_escape_string($obj->feature),
									mysql_real_escape_string($obj->location), 
									mysql_real_escape_string($obj->url)
						 		   );			
		}
		else
		{
			$this->query = sprintf($update, Cal_Database::$tables[events],
								  	$obj->name,
								  	$obj->type,
								  	$obj->feature,
								  	$obj->location,
								  	$obj->description,
								  	$obj->fee,
								  	$obj->url,
								  	$obj->get_id()
								   );
		}
		
		if( $result = mysql_query($this->query) )
		{
			// If it was an insert, get new id and update object
			if( mysql_insert_id() != 0 )
				$obj->set_id( mysql_insert_id() ) ;
				
			$this->refresh(&$obj) ;
		}
		
		return $result ;	
	}
	
	function delete($obj)
	{
		$delete = 'DELETE FROM %s WHERE id="%s"' ;
		
		if( $this->exists($obj) )
		{
			$this->query = sprintf($delete, Cal_Database::$tables[events],
											$obj->get_id()
								  );
		}
		else
			$this->query = '' ;
			
		return mysql_query($this->query) ;		
	}
	
	function refresh($obj)
	{
		$refresh = 'SELECT * FROM %s WHERE id = "%s"' ;
		
		if( $this->exists($obj) )
		{
			$this->query = sprintf($refresh, Cal_Database::$tables[events],
											 $obj->get_id()
								  );
			$result = mysql_query($this->query) ;
			
			while( $event = mysql_fetch_assoc($result) )
			{
				$obj->name         = $event[Name] ;
				$obj->type         = $event[Type] ;
				$obj->feature      = $event[Feature] ;
				$obj->location     = $event[Location] ;
				$obj->description  = $event[Description] ;
				$obj->fee          = $event[Fee] ;
				$obj->url          = $event[Url] ;
			}
			
			return true ;
		}
		else
			return false ;
	}
}

/**
* ScheduleManager Class: Manages dates in the database
*/

class ScheduleManager implements iCalManage
{	
	private $query ;
	
	function exists($obj)
	{
		$find = "SELECT * FROM %s WHERE date = '%s'" ;
		$this->query = sprintf($find, mysql_real_escape_string(Cal_Database::$tables[calendar]),
									  mysql_real_escape_string($obj->date)
					    	  );
		$result = mysql_query($this->query) ;
		
		if(!$result)
		{
			$msg = 'Invalid query: ' . mysql_error() . "<br>" ;
			$msg .= 'Whole query: ' . $this->query ;
			die($msg) ;
		}
		
		if( $result = mysql_fetch_assoc($result) )
			return true ;
		else
			return false ;			
	}
	
	function roster()
	{
		$roster = 'SELECT * FROM %s' ;
		$scheduleList = array() ;
		
		$this->query = sprintf($roster, Cal_Database::$tables[calendar]) ;
		
		if( $result = mysql_query($this->query) )
		{
			while($row = mysql_fetch_assoc($result))
			{
				$event = new Event() ; 
				$event->set_id($row[event]) ;
				Cal_Management::$Manage->Events->refresh($event) ;
				
				$schedule = new Schedule() ;
				$schedule->set_id($row[id]) ;
				$schedule->date = $row[Date] ;
				$schedule->event = $event ;
				 				
				$scheduleList[] = $schedule ;
			}
		}
		
		return $scheduleList ;
	}
	
	function roll($obj)
	{
		$dateRoster = 'SELECT * FROM %s where event = "s"' ;
		$eventRoster = 'SELECT * FROM %s where Date = "%s"' ;
		$List = array() ;
		
		if( get_class($obj) == 'Event' )
		{
			$this->query = sprintf($dateRoster, Cal_Database::$tables[calendar],
												  $obj->get_id()
								   );
			
			if( $result = mysql_query($this->query) )
			{
				Cal_Management::$Manage->Events->refresh($obj) ;				
				while($row = mysql_fetch_assoc($result))
				{
					$schedule = new Schedule() ;
					$schedule->set_id($row[id]) ;
					$schedule->date = $row[Date] ;
					$schedule->event = $obj ;
			
					$List[] = $schedule ;
				}
			}
		}
		else if( get_class($obj) == 'Schedule' )
		{
			$this->query = sprintf($eventRoster, Cal_Database::$tables[calendar],
												 $obj->date
								   );
			
			if( $result = mysql_query($this->query) )
			{
				while($row = mysql_fetch_assoc($result))
				{
					$event = new Event() ;
					$event->set_id($row[event]) ;
					Cal_Management::$Manage->Events->refresh($event) ;
				
					$List[] = $event ;
				}
			}
		}
		
		return $List ;
	}
	
	function save($obj)
	{
		$insert = 'INSERT INTO %s VALUES(NULL, "%s", "%s")' ;
		
		$this->query = sprintf($insert, Cal_Database::$tables[calendar],
								  $obj->date,
								  $obj->event->get_id()
							   );			

		if( $result = mysql_query($this->query) )
		{
			// If it was an insert, get new id and update object
			if( mysql_insert_id() != 0 )
				$obj->set_id( mysql_insert_id() ) ;
				
			$this->refresh(&$obj) ;
		}
		
		return $result ;
	}
	
	function delete($obj)
	{
		$delete = 'DELETE FROM %s WHERE id = "%s"' ;
		
		if( $obj->get_id() )
		{
			$this->query = sprintf($delete, Cal_Database::$tables[calendar],
											$obj->get_id()
								  );
		}
		else
			$this->query = '' ;
			
		return mysql_query($this->query) ;
	}
	
	// Schedules can only be refreshed if object id is set
	function refresh($obj)
	{
		$refresh = 'SELECT * FROM %s WHERE  id = "%s"' ;
		
		if( $obj->get_id() )
		{
			$this->query = sprintf($refresh, Site_Database::$tables[group],
											 $obj->get_id()
								  );
			$result = mysql_query($this->query) ;
			
			while( $schedule = mysql_fetch_assoc($result) )
			{
				$event = new Event() ;
				$event->set_id($schedule[event]) ;
				Cal_Management::$Manage->Events->refresh($event) ;
				
				$obj->date = $schedule[Date] ;
				$obj->event = $event ;
			}
			return true ;
		}
		else
			return false ;
	}	
}

?>

<?php Cal_Management::setup() //Configure Management ?>