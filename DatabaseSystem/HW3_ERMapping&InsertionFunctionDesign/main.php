<!DOCTYPE html>
<?php 
	session_start(); 
?>
<html>

<head>
</head>

<body>
<?php

	$host    ="localhost";
	$username="dbuser";
	$password="dbuser";
	$database="db_pj3";
	$con=mysqli_connect($host, $username, $password, $database);
	
	if(isset($_POST["user"]) && isset($_POST["pw"]))
	{
	    $sql=
            "SELECT name 
             FROM   users 
             WHERE  uid     ='".mysqli_real_escape_string($con, $_POST['user'])."' 
	     AND    password='".mysqli_real_escape_string($con, md5($_POST['pw']))."' ";
	}
	else if(isset($_SESSION['login_user']) && isset($_SESSION['login_pw']))
	{
            $sql=
            "SELECT users.name 
             FROM   users 
             WHERE  uid     ='".mysqli_real_escape_string($con, $_SESSION['login_user'])."' 
	     AND    password='".mysqli_real_escape_string($con, md5($_SESSION['login_pw']))."' ";
	}
        else
        {
            die ("<p align=center><font color=#ff8000>----------------------------------------------</font></p>
              <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
	          <p align=center><font color=#ff8000>----------------------------------------------</font></p><h2>
                  <p align='center'><font color=blue>You have not logged in yet!</font></p></h2>
                  <h3><p align='center'><a href=index.php>Login</a></p></h3>");
        
        }
	
	$user=mysqli_query($con, $sql);
	
	if($row=mysqli_fetch_array($user))
	{
	    if(!isset($_SESSION['login_user']) || !isset($_SESSION['login_pw']))
            {
		$_SESSION['login_user']=$_POST['user'];
		$_SESSION['login_pw']  =$_POST['pw'];
            }
            
            if(isset($_GET['like']))
             {
                 if($_GET['like']==1)
                 {
                     $sql=
                    "INSERT INTO likes 
                     (postid, uid)
                     VALUES ('$_GET[post]','$_SESSION[login_user]')";
                     mysqli_query($con, $sql);    
                 }
                 else if($_GET['like']==-1)
                 {
                     $sql=
                    "DELETE FROM likes 
                     WHERE       uid='$_SESSION[login_user]'
                     AND         postid='$_GET[post]'";
                     mysqli_query($con, $sql);    
                 }
             }
	
             if(isset($_GET['input_text']) && $_GET['input_text']!=NULL 
                && (!isset($_SESSION['input_text']) || $_SESSION['input_text']!=$_GET['input_text']))
             {
                 $_SESSION['input_text']=$_GET['input_text'];
                 //echo $_GET['input_text'];
                 $sql0=
                 "SELECT MAX(postid) as max
                  FROM   articles";
                 $max_postid=mysqli_query($con, $sql0);
                 $max_postid=mysqli_query($con, $sql0);
                 if($row1=mysqli_fetch_array($max_postid))
                 {
                     //echo $row['max'];
                     $sql=
                    "INSERT INTO articles 
                     (uid, postid, content, time)
                    VALUES ('$_SESSION[login_user]', '$row1[max]'+1, '$_GET[input_text]', NOW())";
                    mysqli_query($con, $sql);
                 }
                 unset($_GET['input_text']);
             }
            
             echo
             "<table bordercolor=white align=center width=900>
              <tr>
                 <th bgcolor=orange colspan=3 align=left>
                 <h2><font style='background-color:orange'> &nbsp Welcome <i>".$row['name']."</i> !</font></h2>
                 &nbsp <a href=userinfo.php?n=$_SESSION[login_user]>User Info</a> &nbsp <a href=logout.php>Log Out</a></th>
              </th>
              <tr><th colspan=3><hr></th>
              <tr><th align=left>Say something...</th>
              <tr><th colspan=3 align=left>
                 <form method=get action=main.php>
                 <textarea rows=6 cols=59 name='input_text'></textarea>
                 <input type='submit'>
                 </form> 
              </th>
              <tr>
              <tr><th align=left>";
               
                $sql=
		"SELECT DISTINCT users.name AS user, articles.postid AS post, articles.content AS text, articles.time AS time
		 FROM            users, friends, articles
                 WHERE          (friends.uid='$_SESSION[login_user]'		
                                 AND friends.friend_id=users.uid		
                                 AND articles.uid=friends.friend_id)
                                 OR 
                                (users.uid='$_SESSION[login_user]' 
                                 AND articles.uid=users.uid)
                 ORDER BY       time DESC";
                
                $article=mysqli_query($con, $sql);	
		while($row=mysqli_fetch_array($article))
                {
                    echo $row['user']." said:<br/>";
		    echo "<table bordercolor=black width=500><th><p align=left>$row[text]</p></th></table>";
                    
                    $sql=
                    "SELECT uid
                     FROM   likes
                     WHERE  postid='$row[post]'";
                    $like=mysqli_query($con, $sql);	
                    echo 
                    "<table>
                     <th align=left width=350>".mysqli_num_rows($like).' ';
                    $sql=
                    "SELECT uid
                     FROM   likes
                     WHERE  uid='$_SESSION[login_user]'
                     AND    postid='$row[post]'";
                    $Ilike=mysqli_query($con, $sql);
                    if(mysqli_fetch_array($Ilike))
                    {
                         echo
                         "<a href=main.php?like=-1&post=$row[post]>Unlike</a></th>";
                    }
                    else
                    {       
                         echo
                         "<a href=main.php?like=1&post=$row[post]>Like</a></th>";
                    }           
		    echo "<th>$row[time]</th></table>";
		    echo "<br/><br/>";
                }
             
            echo
            "</th>
             <th width=200></th>
             <th valign=top>
             <table bordercolor=white align=left>
             <tr><th align=left><h2>Friends</h2></th>";
                
                $sql=
		"SELECT users.uid, users.name 
		 FROM   users, friends
		 WHERE  friends.uid='$_SESSION[login_user]'
		 AND    friends.friend_id=users.uid";
                
                 $friend=mysqli_query($con, $sql);
		 while($row=mysqli_fetch_array($friend))
                 {
		    echo "<tr><th align=left><a href=userinfo.php?n=$row[uid]>$row[name]</a></th>";
		 }
             echo
            "</table></th>
             </table>";
	}
        else
        {
            die ("<p align=center><font color=#ff8000>----------------------------------------------</font></p>
              <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
	          <p align=center><font color=#ff8000>----------------------------------------------</font></p><h2>
                  <p align='center'><font color=blue>Wrong Password!</font></p></h2>
                  <h3><p align='center'><a href=index.php>Login</a></p></h3>");
        }
        
	mysqli_close($con);
?>	

</body>

</html>