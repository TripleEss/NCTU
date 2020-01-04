<!DOCTYPE html>
<?php 
	session_start(); 
?>
<html>

<head>
	<title>Facenote</title>
</head>

<body>
<?php
    if(!$_POST)
    { 
        echo
	"
            <p align=center><font color=#ff8000>----------------------------------------------</font></p>
        <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
            <p align=center><font color=#ff8000>----------------------------------------------</font></p>
	
	<h2><p align=center>Create a new group</p></h2>
		
	<form method=post action=create_group.php>
	<table bordercolor=white align=center>
	<tr>
	<th><p align=right>Name:</p></th>   
	<th><input type=text     name='newgroup'/></th>
	<tr>
	<th><p align=right>Introduction:</p></th>
	<th><textarea rows=10 cols=17 name='newgroup_intro'></textarea></th>
        <tr><th align=center colspan=2><input type=submit></th>
	<form method=post action=create_group.php>
        <tr><h3><p align='center'><a href=main.php?n=$_SESSION[login_user]>Main Page</a></p></h3>";
    }
    else
    {

	$host    ="localhost";
	$username="db_user";
	$password="db_user";
	$database="db_0016046";
	$con=mysqli_connect($host, $username, $password, $database);
	
	if(isset($_POST["newgroup"])
	&& isset($_POST["newgroup_intro"]) 
        && $_POST["newgroup"]!=NULL
        && $_POST["newgroup_intro"]!=NULL)
	{
                 $sql0=
                 "SELECT MAX(gid) as max
                  FROM   groups";
                 
                 $max_gid=mysqli_query($con, $sql0);
                 if($row1=mysqli_fetch_array($max_gid))
                 {
                     //echo $row['max'];
                     $sql=
                    "INSERT INTO groups 
                     (gid, name, intro, admin)
                     VALUES ('$row1[max]'+1, '$_POST[newgroup]', '$_POST[newgroup_intro]', '$_SESSION[login_user]')";
                     mysqli_query($con, $sql);
                     
                     $sql=
                    "INSERT INTO join_groups 
                     (uid, gid)
                     VALUES ('$_SESSION[login_user]', '$row1[max]'+1)";
                     mysqli_query($con, $sql);
                     
                      echo
                    "
                       <p align=center><font color=#ff8000>----------------------------------------------</font></p>
                    <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                        <p align=center><font color=#ff8000>----------------------------------------------</font></p>
                        
                         <h2><p align='center'><font color=blue>Create Success!</font></p></h2>
                    <h3><p align='center'><a href=main.php?n=$_SESSION[login_user]>Main Page</a></p></h3>";
                 }
        }
        else
        {
            echo
            "
                <p align=center><font color=#ff8000>----------------------------------------------</font></p>
            <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
                <p align=center><font color=#ff8000>----------------------------------------------</font></p>

            <h2><p align='center'><font color=blue>Not finish!</font></p></h2>
            
             <form method=post action=create_group.php>
            <table bordercolor=white align=center>
            <tr>
            <th><p align=right>Name:</p></th>   
            <th><input type=text     name='newgroup'/></th>
            <tr>
            <th><p align=right>Introduction:</p></th>
            <th><textarea rows=10 cols=17 name='newgroup_intro'></textarea></th>
            <tr><th align=center colspan=2><input type=submit></th>
            <form method=post action=create_group.php>
            <tr><h3><p align='center'><a href=main.php?n=$_SESSION[login_user]>Main Page</a></p></h3>";
        }
        
        mysqli_close($con);
    }

?>
</body>

</html>