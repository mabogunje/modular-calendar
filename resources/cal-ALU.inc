<?php // These are the ALU classes for the calendar class

/**
* Day Class 
* Represents one day of events in the calendar. *Does not support hourly event slots
* property@ length: Integer number of seconds in a Day
* property@ timestamp: Timestamp datetime information for day
* property@ events: Event Array
*/
class Day
{
	private $length = 86400 ;
	
	function __construct()
	{
		$this->timestamp = time() ;
		$this->events = array() ;
	}
	
	/**
	* get_length() Method: returns the length of a day in seconds
	* return this->length
	*/
	
	public function get_length()
	{
		return $this->length ;
	}
	
	/**
	* get_date($format) Method: Returns the day's date as a string according to the format param.
	* Default format "April 1st 2008"
	*
	* param@ format: Date format string
	* return: formatted date string based on day->timestamp
	*/
	
	public function get_date($format=NULL)
	{
		if($format == NULL)
			return date('l, jS F', $this->timestamp) ;
		else
			return date($format, $this->timestamp) ;
	}
	
	/** 
	* get_timestamp Method: Returns day's timestamp
	* return this->timestamp
	*/
	
	public function get_timestamp()
	{
		return $this->timestamp ;
	}
	
	/**
	* set_timestamp(time) Method: Set's day's timestamp to time
	* param@ time: New timestamp
	*/
	
	public function set_timestamp($time)
	{
		if(is_numeric($time))
			$this->timestamp = $time ;
	}
	
	/**
	* add(value) Method: Shifts day forward by value i.e day->add(1) sets April 1st to April 2nd
	* param@ value: Number of days by which to shift this day
	*/
	
	public function add($value)
	{
		if(is_numeric($value))
		{
			$days = $this->length * $value ;
			$this->timestamp += $days ;
		}
	}
	
	/**
	* deduct(value) Method: Shifts day backward by value i.e day->deduct(1) sets April 2nd to April 1st
	* param@ value: Number of days by which to shift this day
	*/
	public function deduct($value)
	{
		if(is_numeric($value))
		{
			$days = $this->length * $value ;
			$this->timestamp -= $days ;
		}
	}
	
	/** 
	* equals(day) Method: Sets this day equal to another day ( Deep copy )
	* param@ day: Day to copy from
	*/
	
	public function equals($day)
	{
		$this->timestamp = $day->timestamp ;
		$this->events = $day->events ;
	}
}

/**
* Week Class: Represents a week in the calendar
* property@ length: Integer length of a week in days
* property@ days: Array of Day objects in week
*/

class Week
{
	private $length = 7 ;
	
	function __construct()
	{
		$this->days = array() ;
		$this->days[0] = NULL ; //indexed from 1

		// Direct Access
		$this->firstDay ;
		$this->lastDay ;
	}
	/**
	* get_length() Method: Returns the standard length of a week
	* return this->length
	*/
	
	function get_length()
	{
		return $this->length ;
	}
	
	/** 
	* num_days() Method: Returns the actual length of this week (in days)
	* return counter: num of days in this week
	*/
	
	function num_days()
	{
		$counter = count($this->days)-1 ;
		return $counter ;
	}
}

/**
* Month Class: Represents a month in the calendar
* property@ daysPwk: Integer Standard Number of days in a week
* property@ length: Integer Number of days in this month
* property@ weeks: Integer Number of weeks in this month
* property@ days: Array of Day objects in the month
*/

class Month
{	
	function __construct()
	{	
		$this->daysPwk = 7 ;
		$this->length = date('t') ;
		$this->weeks = ceil($this->length / $this->daysPwk );
		$this->days = array() ;
		$this->days[0] = NULL ; // indexed from 1
		
		for($i=1; $i<= $this->length; $i++)
		{
			$day = new Day() ;
			$day->set_timestamp( mktime(0,0,0,date('n'),$i,date('Y')) ) ;
			$this->days[] = $day ;
		}
		
		// Direct Access
		$this->firstDay = &$this->days[1] ;
		$this->lastDay  = &$this->days[$this->length] ;
	}
	
	/**
	* set_month(month, year) Method: Sets the month and year of this month and refreshes the days in the month accordingly
	* param@ month: Integer representation of month i.e January = 1|01
	* param@ year: 4 digit representation of year. Default is current year
	*/
	
	public function set_month( $month, $year=NULL)
	{
		if($year == NULL)
			$year = date('Y') ;
		$this->length = date('t', mktime(0,0,0,$month,1,$year)) ;
		
		// clear prior day entries
		$this->days = array_fill(1,$this->length,NULL) ;		
		
		for($i=1; $i<=$this->length; $i++)
		{
			$day = new Day() ;
			$day->set_timestamp( mktime(0,0,0,$month,$i,$year) ) ;
			$this->days[$i] = $day ;
		}
		
		// Direct Access
		$this->firstDay = &$this->days[1] ;
		$this->lastDay  = &$this->days[$this->length] ;
	}
	
	/**
	* get_name(format) Method: Returns the month name according to format. Format may be full (F) or abbreviated (M)
	* param@ format: Format by which to return the month name
	*/
	
	public function get_name($format)
	{
		if($format == 'F' || $format == 'M')
			return $this->days[1]->get_date($format) ;
	}
	
	/**
	* get_pos(format) Method: Return the number value of month according to format i.e January = 1|01
	* param@ format: Format by which to return value. May be long (n = 1) or short(m = 01)
	*/
	
	public function get_pos($format='n')
	{
		if($format == 'n' || $format == 'm')
			return $this->days[1] -> get_date($format) ;
	}
	
	/**
	* get_week(pos): Returns a week object of the pos week in the month i.e for pos=2 get_week returns 2nd week in month
	* param@ pos: The position of the week in the month to return 
	*/
	
	public function get_week($pos)
	{
		if($pos <= $this->weeks)
		{
			$week = new Week() ;
			$start = (($pos-1)*$week->get_length())+1 ; // The day after the last day of the previous week
			$end = $pos * $week->get_length() ; 		// The last day of the current week
			
			if($end > $this->length)
				$end = $this->length ;
					
			if($start <= $end)
			{
				for($i=$start; $i<=$end; $i++)
					$week->days[] = $this->days[$i] ;
			}

			$week->firstDay = &$week->days[1] ;
			$week->lastDay  = &$week->days[$week->num_days()] ;
			return $week ;
		}
	}
}

/**
* Term Class: Represents a Term in the calendar
* property@ terms: Associative array of all term names (i.e Fall->Summer) and term start day dates i.e 8 = 8th of first month in term
* property@ info: Date info array of current date info
* property@ months: Array of month objects in term
* property@ term: Name of current term i.e Fall
* property@ start: Month number of start month i.e 1 = January
* property@ end: Month number of end month i.e 12 = December 
*/
class Term
{
	private $terms = array(NULL, "Fall"=>25, "Winter"=>8, "Spring"=>2, "Summer"=>25) ;
	
	function __construct()
	{
		$this->info = getdate() ;
		$this->months = array() ;
		$this->months[0] = NULL ;
		$this->term ;
		$this->length ;
		$this->start ; 
		$this->end ;
		
		switch($this->info[mon])
		{
			case 1:
			case 2:
			case 3:
				$this->term = "Winter" ;
				$this->start = 1; $this->end = 3 ;
				break ;
			case 4:
			case 5:
			case 6:
				$this->term = "Spring" ;
				$this->start = 3 ; $this->end = 6 ;
				break ;
			case 7:
			case 8:
			case 9:
				$this->term = "Summer" ;
				$this->start = 6 ; $this->end = 9 ;
				break ;
			case 10:
			case 11:
			case 12:
				$this->term = "Fall";
				$this->start = 9 ; $this->end = 12 ;
				break ;
		}
		$month = new Month() ;
		$this->length = ($this->end - $this->start) + 1 ;
		
		if($this->info[mday] < $this->terms[$this->term] && $month->get_pos() <= $this->start)
		{
			$this->start -= ($this->length - 1) ;
			$this->end -= ($this->length - 1) ;
		}
						
		for($i=$this->start; $i<=$this->end; $i++)
		{
			$month = new Month() ;
			$month->set_month($i) ;
			$this->months[] = $month ;
		}
		
		// Direct Access
		$this->firstDay = &$this->months[1]->firstDay ;
		$this->lastDay  = &$this->months[$this->length]->lastDay ;
	}
	
	/**
	* set_term(term) Method: Changes the current term to "term" and refreshes months, weeks, days, and events therein
	* param@ term: The term to change to; Value must match one of names in this->terms to be accepted
	*/
	
	public function set_term($term)
	{
		$this->startday ;
		if($term == "Winter")
		{
			$this->term = $term ;
			$this->startday = 8 ;
			$this->start = 1;
			$this->end = 3 ;	
		}
		if($term == "Spring")
		{
			$this->term = $term ;
			$this->startday = 2 ;
			$this->start = 3 ;
			$this->end = 6 ;
		}
		if($term == "Summer")
		{
			$this->term = $term ;
			$this->startday = 25 ;
			$this->start = 6;
			$this->end = 9 ;	
		}
		if($term == "Fall")
		{
			$this->term = $term ;
			$this->startday = 25 ;
			$this->start = 9;
			$this->end = 12 ;	
		}
		
		if($this->startday < $this->terms[$this->term])
		{
			$this->start -= ($this->length - 1) ;
			$this->end -= ($this->length - 1) ;
		}
		
		
		//empty array first
		for($i=1; $i<=$this->length; $i++)
			array_pop($this->months) ;
		
		//refill array with new months
		$this->length = ($this->end - $this->start) + 1 ;
		for($i=$this->start; $i<=$this->end; $i++)
		{
			$month = new Month() ;
			$month->set_month($i) ;
			$this->months[] = $month ;
		}
		
		// Direct Access
		$this->firstDay = &$this->months[1]->firstDay ;
		$this->lastDay  = &$this->months[$this->length]->lastDay ;
	}
	
	/**
	* get_name() Method: Returns the name of the current term
	* return this->term
	*/
	
	function get_name()
	{
		return $this->term ;
	}
	
	/**
	* get_length() Method: Returns the length of the current term (in months)
	* return this->length
	*/
	
	function get_length()
	{
		return $this->length ;
	}
}

?>