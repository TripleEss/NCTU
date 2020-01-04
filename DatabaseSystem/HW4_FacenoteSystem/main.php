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
	$username="db_user";
	$password="db_user";
	$database="db_0016046";
	$con=mysqli_connect($host, $username, $password, $database);
	
        // initial login
	if(isset($_POST["user"]) && isset($_POST["pw"]))
	{
	    $sql=
            "SELECT name 
             FROM   users 
             WHERE  uid     ='".mysqli_real_escape_string($con, $_POST['user'])."' 
	     AND    password='".mysqli_real_escape_string($con, md5($_POST['pw']))."' ";
	}
        // logged
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
            // set login session
	    if(!isset($_SESSION['login_user']) || !isset($_SESSION['login_pw']))
            {
		$_SESSION['login_user']=$_POST['user'];
		$_SESSION['login_pw']  =$_POST['pw'];
            }
            
            // if click like?
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
	
             // if input text?
             if(isset($_GET['input_text']) && $_GET['input_text']!=NULL 
                && (!isset($_SESSION['input_text']) || $_SESSION['input_text']!=$_GET['input_text']))
             {
                 $_GET['input_text']=mysqli_real_escape_string($con, $_GET['input_text']);
                 $_SESSION['input_text']=$_GET['input_text'];
                 
                 $sql0=
                 "SELECT MAX(postid) as max
                  FROM   articles";

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
             
              // if input response?
             if(isset($_GET['input_res']) && $_GET['input_res']!=NULL 
                && (!isset($_SESSION['input_res']) || $_SESSION['input_res']!=$_GET['input_res']))
             {
                 $_GET['input_res']=mysqli_real_escape_string($con, $_GET['input_res']);
                 $_SESSION['input_res']=$_GET['input_res'];
                 
                 $sql=
                 "INSERT INTO responses 
                  (uid, postid, content, time)
                  VALUES ('$_SESSION[login_user]', '$_GET[respost]', '$_GET[input_res]', NOW())";
                 
                 mysqli_query($con, $sql);
                 
                 unset($_GET['input_res']);
             }
            
             // if delete text?
             if(isset($_GET['del']) && $_GET['del']==1)
             {
                 //echo $_GET['post'];
                 $sql0=
                 "DELETE FROM articles
                  WHERE  articles.postid='$_GET[post]'";
                 mysqli_query($con, $sql0);
                     
                  $sql0=
                 "DELETE FROM likes
                  WHERE  likes.postid='$_GET[post]'";
                 mysqli_query($con, $sql0);
                
                unset($_GET['del']);
             }

             // if delete response?
             if(isset($_GET['del']) && $_GET['del']==2)
             {
                $_GET['delrestext']=mysqli_real_escape_string($con, $_GET['delrestext']);
                
                 $sql0=
                 "DELETE FROM responses
                  WHERE  responses.postid='$_GET[delpost]' AND responses.uid='$_SESSION[login_user]' 
                     AND responses.content='$_GET[delrestext]'";
                 mysqli_query($con, $sql0);
               
                unset($_GET['delres']);
             }

             // welcome
             echo
             "<table bordercolor=white align=center width=900>
              <tr>
                 <th bgcolor=orange colspan=3 align=left>
                 <h2><font style=background-color:orange> &nbsp Welcome <i>".$row['name']."</i> !</font></h2>
                 &nbsp <a href=userinfo.php?n=$_SESSION[login_user]>User Info</a> &nbsp <a href=logout.php>Log Out</a></th>        
              <tr>
                 <th colspan=3><hr></th>
              <tr>
                 <th align=left>Say something...</th>
              <tr>
                 <th colspan=3 align=left>
                 <form method=get action=main.php>
                 <textarea rows=6 cols=59 name='input_text'></textarea>
                 <input type='submit'>
                 </form></th>
              <tr>
              <tr>
                 <th align=left valign=top>";
                // articles
                $sql=
		"SELECT DISTINCT users.uid AS id, users.name AS user, articles.postid AS post, articles.content AS text, articles.time AS time, articles.gid AS gid
		 FROM            users, friends, articles, join_groups
                 WHERE          (articles.gid IS NULL
                                 AND friends.uid='$_SESSION[login_user]'		
                                 AND friends.friend_id=users.uid		
                                 AND articles.uid=friends.friend_id)
                                 OR
                                 (articles.gid IS NOT NULL
                                 AND join_groups.uid='$_SESSION[login_user]'		
                                 AND join_groups.gid=articles.gid
                                 AND friends.uid='$_SESSION[login_user]'		
                                 AND friends.friend_id=users.uid		
                                 AND articles.uid=friends.friend_id)
                                 OR
                                (users.uid='$_SESSION[login_user]' 
                                 AND articles.uid=users.uid)                              
                 ORDER BY       time DESC";
                
                $article=mysqli_query($con, $sql);
                
             
		while($row=mysqli_fetch_array($article))
                {
                    echo "<table bordercolor=white>
                          <tr>
                             <th align=left width=430>
                             <i>$row[user]</i>";    
                    
                    if($row['gid']!=NULL)
                    {
                        $sql0=
                        "SELECT name
                         FROM   groups
                         WHERE  gid='$row[gid]'";
                        $g=mysqli_query($con, $sql0);
                        while($row0=mysqli_fetch_array($g))
                        {                  
                            echo " in <i>".$row0['name']."</i> ";
                        }
                    }
                    echo  " said:</th>";
                    if($row['id']==$_SESSION['login_user'])
                    {
                        echo "<th align=center>
                              <input type='button' name='delete_text' value='Delete' onclick=location.href='main.php?del=1&post=$row[post]'></th>";                       
                    }
                       
                    echo "</table>";
		    echo "<table bordercolor=red width=500>
                          <tr>
                             <th><p align=left>$row[text]</p></th>
                          </table>";
                    
                    $sql=
                    "SELECT uid
                     FROM   likes
                     WHERE  postid='$row[post]'";
                    $like=mysqli_query($con, $sql);	
                    echo 
                    "<table>
                     <tr>
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
		    echo "<th align=right>$row[time]</th>
                          </table>";
                    
                    
                    $sql0=
                    "SELECT DISTINCT responses.uid AS res_id, users.name AS user, responses.content AS res_text, responses.time AS res_time
                     FROM            users, responses 
                     WHERE           responses.postid='$row[post]' AND responses.uid=users.uid
                     ORDER BY        res_time ASC";
                    
                    $response=mysqli_query($con, $sql0);
                    
                    while($row0=mysqli_fetch_array($response))
                    {
                        echo "<table bordercolor=pink>
                              <tr>
                                 <th align=left width=130>
                                 <font color=gray>$row0[user]</font></th>
                                 <th align=left width=150>$row0[res_text]</th>
                                 <th align=right>$row0[res_time]</th>";

                        if($row0['res_id']==$_SESSION['login_user'])
                        {
                            echo "<th>
                                  <form method=get action=main.php>
                                  <input type=submit name=delete_res value=Delete>
                                  <input type=hidden name=del value=2>
                                  <input type=hidden name=delpost value=$row[post]>
                                  <input type=hidden name=delrestext value=$row0[res_text]></form></th></table>";                       
                        }
                        else
                        {
                            echo "<th width=57></th></table>";
                        }

                    }
                    echo "<table>
                          <tr>
                             <th align=left>
                             <form method=get action=main.php>
                             <textarea rows=2 cols=52 name=input_res></textarea>
                             <input type=hidden name=respost value=$row[post]>
                             <input type='submit'>
                             </form></th>
                          </table>";
                    echo "<br/><br/><br/><br/>";
                }
             
            echo
            "</th>
             <th width=200></th>
             <th valign=top>
             <table bordercolor=white align=left>
             <tr><th align=left><h2>Friends</h2></th>";
                
                // friends
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
            "<tr><th align=left><h2>Groups</h2></th>";
                
                // friends
                $sql=
		"SELECT groups.gid, groups.name 
		 FROM   groups, join_groups
		 WHERE  join_groups.uid='$_SESSION[login_user]'
		 AND    join_groups.gid=groups.gid";
                
                 $group=mysqli_query($con, $sql);
		 while($row=mysqli_fetch_array($group))
                 {
		    echo "<tr><th align=left><a href=group.php?n=$row[gid]>$row[name]</a></th>";
		 }
             echo
             "<tr><th><h2></h2></th><tr><th align=left><a href=create_group.php>Create Group</a></th></table>";
	}
        else
        {
            die ("<p align=center><font color=#ff8000>----------------------------------------------</font></p>
              <h1><p align=center><font color=#ff0000>                 <b>FACENOTE</b>              </font></p></h1>
	          <p align=center><font color=#ff8000>----------------------------------------------</font></p><h2>
                  <p align='center'><font color=blue>Wrong Account or Password!</font></p></h2>
                  <h3><p align='center'><a href=index.php>Login</a></p></h3>");
        }
        
	mysqli_close($con);
?>	

</body>

</html>