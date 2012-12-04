<?php // Calendar example ?>
<link rel="stylesheet" type="text/css" href="themes/default.css" />
<?php include 'calendar.php'; ?>

<?php
  $calendar = new Calendar($_GET[view], $_GET['time']);
  $calendar->width = "100%";
  $calendar->height = "50%";
  $calendar->display();
?>
