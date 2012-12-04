<?php // Calendar class: Overall controlling object for the Calendar App ?>

<?php
	// Including Building Classes  
	require_once('config/Settings.php') ;
	require_once('resources/management.inc') ;
	require_once('resources/cal-ALU.inc') ; 
	require_once('resources/cal-GUI.inc') ; 

?> 

<?php
/** 
* iCalendar Interface: Defines how all Calendars should interact
* build() Method: Builds calendar object complete with information in each cell object based on the view
* toString() Method: Converts the build to an html string
* display() Method: Writes the calendar to the page
*/ 

interface iCalendar
{
	public function build() ;
	public function toString() ;
	public function display() ;
}

/** 
* Calendar Class: Wrapper for all calendar views, instantiates different calendar based on v,time, and edit params
* property@ v: Calendar view to use; Acceptable value is only one which maches an element is views array; Default view is "month"
* property@ time: Timestamp for that calendar from which Calendar Date is determined
* property@ edit: If set to true, all calendar controls (buttons and forms) are visible
* property@ views: Associative array of view names->integer value i.e enumerated type
* property@ obj: The logic object for the current Calendar view (refer to resource/cal-ALU)
* property@ calendar: The calendar object for the current view 
* property@ width: Width of the calendar
* property@ height: Height of the calendar
* property@ controls: Control buttons for switching calendar views
*/ 

class Calendar implements iCalendar
{	
	private $views = array( "day"=>0, "week"=>1, "month"=>2, "term"=>3, "upcoming"=>4, "featured"=>5 );
	protected $obj ;
	public $calendar ;
	
	function __construct($v=NULL, $time=NULL, $edit=true)
	{
		if($v == NULL)
			$v = "month" ;
			
		$this->time = $time ;
		
		if($this->time == NULL)
				$this->time = time() ;
		
		$this->edit = $edit ;
		
		// Add View Controls
		$controls_format = "<ul type='none' class='%s'> %s </ul>\n" ;
		$link_format = "<a href='%s?view=%s&time=%s'> %s </a>" ;
		$control_format = "<li class='%s'> %s </li>&nbsp" ;
		$viewNames = array_keys($this->views) ;
		
		foreach($viewNames as $control)
		{
			$link = sprintf($link_format, Settings::cur_page(), $control, $this->time, strtoupper($control)) ;

			if( $control == array_search($this->views[$v], $this->views) )
				$control = sprintf($control_format, Settings::$control_selected, $link) ;
			else
				$control = sprintf($control_format, Settings::$control_class, $link) ;
			
			$list .= $control ;
		}	
		$this->controls = sprintf($controls_format, Settings::$controls_class, $list) ;
				
		switch($this->views[$v])
		{		
			case 0:
				$this->calendar = new DayCalendar($this->time) ;
				break ;
			case 1:
				$this->calendar = new WeekCalendar($this->time) ;
				break ;
			case 3:
				$this->calendar = new TermCalendar($this->time) ;
				break ;
			case 4:
				$this->calendar = new UpcomingCalendar(time()) ;
				break ;
			case 5:
				$this->calendar = new FeatureCalendar(time()) ;
				break ;
			default:
				$this->calendar = new MonthCalendar($this->time) ;
				break ;
		}
		$this->width = &$this->calendar->width ;
		$this->height = &$this->calendar->height ;
	}
	
	/**
	* set_view(v, time) Method: Set's the type (view) of calendar
	* param@ v: String of view to set to e.g day|month|week
	* param@ time: Timestamp of calendar
	* return void
	*/
	
	function set_view($v, $time=NULL)
	{
		if($time == NULL)
				$time = $this->time ;
				
		switch($this->view[$v])
		{
			case 0:
				$this->calendar = new DayCalendar($time) ;
				break ;
			case 1:
				$this->calendar = new WeekCalendar($time) ;
				break ;
			case 3:
				$this->calendar = new TermCalendar($time) ;
				break ;
			case 4:
				$this->calendar = new UpcomingCalendar($time) ;
				break ;
			default:
				$this->calendar = new MonthCalendar($time) ;
				break;
		}
	}
	
	function build()
	{
		$this->calendar->build() ;
	}
	
	function toString()
	{
		return $this->calendar->toString() ;
	}
	
	function display()
	{
		if($this->edit)
			echo $this->controls ;
		
		$this->calendar->display() ;
	}
}

/** 
* DayCalendar Class: Represents a calendar view of one day
* property@ view: Gui object for this calendar
*/
 
class DayCalendar extends Calendar implements iCalendar
{
	function __construct( $time )
	{
		$this->obj  = new Day() ;
		$this->obj->set_timestamp($time) ;	
		$this->view = new Table() ;
		$this->width = &$this->view->width ;
		$this->height = &$this->view->height ;
		
		$this->build() ;	
	}
	
	function build()
	{
		Cal_Database::connect() ;
		
		$day = new Schedule() ;
		$day->date = $this->obj->get_date('Y-m-d') ;
		$this->obj->events = Cal_Management::$Manage->Agenda->roll($day) ;
		
		// Controls
		$link = "<a href='%s?view=%s&time=%s'> %s </a>" ;
		$prev = sprintf($link, Settings::cur_page(), 'day', ($this->obj->get_timestamp() - $this->obj->get_length()), " << ") ;
		$next = sprintf($link, Settings::cur_page(), 'day', ($this->obj->get_timestamp() + $this->obj->get_length()), " >> ") ;
		
		// Set calendar header
		$this->view->heading = $prev . " " . $this->obj->get_date() . " " . $next ;	
			
		// Put events into Gui
		
		if($this->obj->events)
		{
			foreach( $this->obj->events as $event )
			{
				$row = new Row() ;
				
				$cell = new Cell() ;
				$cell->head = $event->name . " " . $event->type ;
				$cell->body = $event->description ;
				
				$row->cells[] = $cell ;
				$this->view->rows[] = $row ;
			}
		}
		else
		{
			$row = new Row() ;
			$cell = new Cell() ;
			
			$cell->body = "No Event Today" ;
			
			$row->cells[] = $cell ;
			$this->view->rows[] = $row ;
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

/** 
* WeekCalendar Class: Represents a calendar view of one week
* property@ view: Gui object for this calendar
*/

class WeekCalendar extends Calendar implements iCalendar
{

	function __construct( $time )
	{
		$this->obj = new Week() ;
		
		// Calculate week in month
		$this->month = new Month() ;
		$this->month->set_month( date(n, $time), date(Y, $time) ) ;
		$this->wk = ceil( date(j, $time) / $this->obj->get_length() ) ;

		// Retrieve week from month
		$this->obj = $this->month->get_week($this->wk) ;
		$this->view = new Table() ;
		$this->width = &$this->view->width ;
		$this->height = &$this->view->height ;
		$this->build() ;
	}
	
	function build()
	{
		Cal_Database::connect() ;
		
		// Retrieve event data for days in week
		for($i=1; $i <= $this->obj->num_days(); $i++)
		{
			$schedule = new Schedule() ;
			$schedule->date = $this->obj->days[$i]->get_date('Y-m-d') ;
			
			$this->obj->days[$i]->events = Cal_Management::$Manage->Agenda->roll($schedule) ;
		}
		
		// Controls
		$link = "<a href='%s?view=%s&time=%s'> %s </a>" ;
		$prev = sprintf($link, Settings::cur_page(), 'week', ($this->obj->days[$this->obj->num_days()]->get_timestamp() - ($this->obj->days[$this->obj->num_days()]->get_length()*$this->obj->num_days()) ), " << ") ;
		$next = sprintf($link, Settings::cur_page(), 'week', ($this->obj->days[$this->obj->num_days()]->get_timestamp() + ($this->obj->days[$this->obj->num_days()]->get_length()) ), " >> ") ;
		
		// Set calendar header
		$this->view->heading = $prev . " " . $this->month->get_name(F) . ", Week " . $this->wk . " " . $next ;
		
		// Put into Gui
		for( $i=1; $i <= $this->obj->num_days(); $i++ )
		{
			$row = new Row() ;
			$cell = new Cell() ;
			
			if( $this->obj->days[$i]->get_date(Ymj) == date(Ymj, time())  )
				$cell->highlighted = true ;
			
			// Insert dates linked to day view
			$link = "<a href='%s?view=day&time=%s'> %s </a>" ;
			$cell->head =  sprintf( $link, 
									Settings::cur_page(), 
									$this->obj->days[$i]->get_timestamp(), 
									$this->obj->days[$i]->get_date(j) 
								   ) ;
			
			if($this->obj->days[$i]->events)
			{
				foreach($this->obj->days[$i]->events as $event)
				{
					$cell->body .= $event->name . "<br>" ;
				}
				$row->cells[] = $cell ;
			}
			else
			{
				$cell->body = "" ;
				$row->cells[] = $cell ;
			}
			$this->view->rows[] = $row ;
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

class MonthCalendar extends Calendar implements iCalendar
{
	function __construct( $time )
	{
		$this->obj = new Month() ;
		$this->obj->set_month( date(n, $time), date(Y, $time) ) ;
		$this->view = new Table() ;
		$this->width = &$this->view->width ;
		$this->height = &$this->view->height ;
		
		$this->build() ;
	}
	
	function build()
	{
		Cal_Database::connect() ;
		
		// Retrieve event data for days in month
		for($i=1; $i <= $this->obj->length; $i++)
		{
			$schedule = new Schedule() ;
			$schedule->date = $this->obj->days[$i]->get_date('Y-m-d') ;
			
			$this->obj->days[$i]->events = Cal_Management::$Manage->Agenda->roll($schedule) ;
		}

		// Set calendar header
		$this->view->heading = $this->obj->get_name(F) ;
		
		// Put into Gui

		for($i=1; $i<=$this->obj->weeks; $i++)
		{
			$row = new Row() ;
			$week = $this->obj->get_week($i) ;

			for($j=1; $j<=$week->num_days(); $j++)
			{
				$cell = new Cell() ;
				
				if( $week->days[$j]->get_date(j) == date(j, time()) )
					$cell->highlighted = true ;
			
				// Insert dates linked to day view
				$link = "<a href='%s?view=day&time=%s'> %s </a>" ;
				$cell->head =  sprintf( $link, 
										Settings::cur_page(),
										$week->days[$j]->get_timestamp(),
										$week->days[$j]->get_date(j)
									   ) ;
			
				if($week->days[$j]->events)
				{
					foreach($week->days[$j]->events as $event)
					{
						$cell->body .= $event->name . "<br>" ;
					}
					$row->cells[] = $cell ;
				}
				else
				{
					$cell->body = "" ;
					$row->cells[] = $cell ;
				}
			}
			$this->view->rows[] = $row ;
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

/** 
* TermCalendar Class: Represents a calendar view of one term
* property@ view: Gui object for this calendar
*/

class TermCalendar extends Calendar implements iCalendar
{
	function __construct( $time )
	{
		$this->obj = new Term() ;
		
		$month = date(n, $time) ;
		switch($month)
		{
			case 1:
			case 2:
			case 3:
				$term = "Winter" ;
				break ;
			case 4:
			case 5:
			case 6:
				$term = "Spring" ;
				break ;
			case 7:
			case 8:
			case 9:
				$term = "Summer" ;
				break ;
			case 10:
			case 11:
			case 12:
				$term = "Fall";
				break ;
		}
		$this->obj->set_term($term) ;
		$this->view = new Table() ;
		$this->width = &$this->view->width ;
		$this->height = &$this->view->height ;
		
		$this->build() ;
	}
	
	function build()
	{
		Cal_Database::connect() ;
		
		// Retrieve day events
		for($i=1; $i<$this->obj->length; $i++)
		{
			for($j=1; $j<=$this->obj->months[$i]->length ; $j++)
			{
				$schedule = new Schedule() ;
				$schedule->date = $this->obj->months[$i]->days[$j]->get_date('Y-m-d') ;
				
				$this->obj->months[$i]->days[$j]->events = Cal_Management::$Manage->Agenda->roll($schedule) ;
			}
		}

		
		// Set calendar header
		$this->view->heading = $this->obj->get_name() . " Term"  ;
		
		// Put into Gui
		$days = array() ;
				
		for($i=1; $i<=$this->obj->length; $i++)
			$days = array_merge($days, $this->obj->months[$i]->days) ;
		
		foreach($days as $day)
		{			
			$row = new Row() ;
			$cell = new Cell() ;
			
			if( $day->get_date(jn) == date(jn, time()) ) //highlight cell is day is today
					$cell->highlighted = true ;
		
			$link = "<a href='%s?view=day&time=%s'> %s </a>" ;
			$cell->head = sprintf($link, Settings::cur_page(), $day->get_timestamp(), $day->get_date()) ;
			
			foreach($day->events as $event)
				$cell->body .= $event->name . "<br>" ;
			
			if( $day->get_timestamp() == $this->obj->firstDay->get_timestamp() )
				$cell->body .= "<br> -- Term Begins -- </br>" ;
			else if( $day->get_timestamp() == $this->obj->lastDay->get_timestamp() )
				$cell->body .= "<br> -- Term Ends -- </br>" ;
			
			if($day->events)
			{
				$row->cells[] = $cell ;
				$this->view->rows[] = $row ;
			}			
		}
		
		if( empty($this->view->rows) )
		{
			$row = new Row() ;
			$cell = new Cell() ;
			
			$cell->head = "Notice" ;
			$cell->body = "No events scheduled for this term" ;
			
			$row->cells[] = $cell ;
			$this->view->rows[] = $row ;		
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

/** 
* UpcomingCalendar Class: Represents a calendar view of upcoming events
* property@ view: Gui object for this calendar
*/

class UpcomingCalendar extends Calendar implements iCalendar
{
	function __construct($time)
	{
		$this->time = $time ;
		$this->view   = new Table() ;
		$this->width  = &$this->view->width ;
		$this->height = &$this->view->height ;
		
		$this->build() ;
	}
	
	function build()
	{
		Cal_Database::connect() ;
		$events = array() ;
		$dates  = array() ;

		// Retrieve day events
		$get_events = "SELECT * FROM %s INNER JOIN %s ON %s.id = event WHERE Date>='%s'" ;
		$query = sprintf( $get_events, 
						  Cal_Database::$tables[calendar], 
						  Cal_Database::$tables[events],
						  Cal_Database::$tables[events],
						  date('Y-m-d', $this->time)
						 ) ;
		$result = mysql_query($query) ;
			
		if($result)
		{
			while( $info = mysql_fetch_array($result) )
			{
				$event = new Event() ;
				$event->name = $info[Name] ;
				$event->type = $info[Type] ;
				$event->feature = $info[Feature] ;
				$event->location = $info[Location] ;
				$event->description = $info[Description] ;
				$event->fee = $info[Fee] ;
				$event->url = $info[Url] ;
				
				$events[] = $event ;
				$dates[] = $info[Date] ;
			}
		}
				
		// Set calendar header
		$this->view->heading = "Upcoming Events" ;
			
		// Put events into Gui
		
		if($events)
		{
			if( count($events) >= Settings::$max_events )
				$max = Settings::$max_events ;
			else
				$max = count($events) ;
				
			for( $i=0; $i<$max; $i++ )
			{
				$row = new Row() ;
				
				$cell = new Cell() ;
				$cell->head = $dates[$i] ;
				$cell->body = $events[$i]->name . " " . $events[$i]->type ;
				
				$row->cells[] = $cell ;
				$this->view->rows[] = $row ;
			}
		}
		else
		{
			$row = new Row() ;
			$cell = new Cell() ;
			
			$cell->head = "Notice" ;
			$cell->body = "No Upcoming Events" ;
			
			$row->cells[] = $cell ;
			$this->view->rows[] = $row ;
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

/** 
* FeatureCalendar Class: Represents a calendar view of featured events
* property@ view: Gui object for this calendar
*/

class FeatureCalendar extends Calendar implements iCalendar
{
	function __construct($time)
	{
		$this->time = $time ;
		$this->view   = new Table() ;
		$this->width  = &$this->view->width ;
		$this->height = &$this->view->height ;
		
		$this->build() ;
	}
	
	function build()
	{
		Cal_Database::connect() ;
		$events = array() ;
		$dates  = array() ;

		// Retrieve day events
		$get_events = "SELECT * FROM %s INNER JOIN %s ON %s.id = event WHERE Feature=true AND Date >= '%s'" ;
		$query = sprintf( $get_events, 
						  Cal_Database::$tables[calendar], 
						  Cal_Database::$tables[events],
						  Cal_Database::$tables[events],
						  date('Y-m-d', $this->time)
						 ) ;
		$result = mysql_query($query) ;
			
		if($result)
		{
			while( $info = mysql_fetch_assoc($result) )
			{
				$event = new Event() ;
				$event->set_id($info[event]) ;
				Cal_Management::$Manage->Events->refresh($event) ;
				
				$events[] = $event ;
				$dates[] = $info[Date] ;
			}
		}
				
		// Set calendar header
		$this->view->heading = "Featured Events" ;
			
		// Put events into Gui
		
		if($events)
		{
			if( count($events) >= Settings::$max_events )
				$max = Settings::$max_events ;
			else
				$max = count($events) ;
				
			for( $i=0; $i<$max; $i++ )
			{
				$row = new Row() ;
				
				$dateCell = new Cell() ; $dateCell->width = "10%" ;			
				$dateCell->head = date( M, strtotime($dates[$i]) ) ;
				$dateCell->body = date( j, strtotime($dates[$i]) ) ;
				
				$mainCell = new Cell() ; $mainCell->width="90%" ;
				$mainCell->head = $events[$i]->type ;
				$mainCell->body = $events[$i]->name ;
				
				$row->cells[] = $dateCell ;
				$row->cells[] = $mainCell ;
				$this->view->rows[] = $row ;
			}
		}
		else
		{
			$row = new Row() ;
			$cell = new Cell() ;
			
			$cell->head = "Notice" ;
			$cell->body = "No Featured Events" ;
			
			$row->cells[] = $cell ;
			$this->view->rows[] = $row ;
		}
	}
	
	function toString()
	{
		return $this->view->As_string() ;
	}
	
	function display()
	{
		echo $this->toString() ;
	}
}

?>