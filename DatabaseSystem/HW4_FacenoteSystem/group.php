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
                     (uid, gid, postid, content, time)
                    VALUES ('$_SESSION[login_user]', '$_GET[n]', '$row1[max]'+1, '$_GET[input_text]', NOW())";
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
             
             // add a new member
            if(isset($_GET['newmember']))
            {   
                $addfail=2;
                $sql=
                "SELECT uid
                 FROM   users
                 WHERE  uid='$_GET[newmember]'";
                                     
                $existed=mysqli_query($con, $sql);
                if($row=mysqli_fetch_array($existed))
                {   
                    $sql=
                    "SELECT uid
                     FROM   join_groups
                     WHERE  gid='$_GET[n]' AND uid='$_GET[newmember]'";
                    $existedf=mysqli_query($con, $sql);
                    if(!$row=mysqli_fetch_array($existedf))
                    {
                        $sql=
                        "INSERT INTO join_groups 
                         (uid, gid)
                         VALUES ('$_GET[newmember]', '$_GET[n]')";
                        mysqli_query($con, $sql);                      
                        
                        $addfail=0;
                    }
                }
                else
                {
                    $addfail=1;
                }
                
                unset($_GET['addmember']);
            }
            
             // delete a member
            if(isset($_GET['delmember']))
            {   
                $delfail=2;
                $sql=
                "SELECT uid
                 FROM   users
                 WHERE  uid='$_GET[delmember]'";
                                     
                $existed=mysqli_query($con, $sql);
                if(mysqli_fetch_array($existed))
                {
                    
                        $sql=
                        "SELECT uid
                         FROM   join_groups
                         WHERE  uid='$_GET[delmember]' AND gid='$_GET[n]'";
                             
                        $existed=mysqli_query($con, $sql);
                         if(mysqli_fetch_array($existed))
                         {
                            $sql0=
                            "SELECT admin
                             FROM groups
                             WHERE  groups.gid='$_GET[n]'";
                            $ad=mysqli_query($con, $sql0);

                            if($row0=mysqli_fetch_array($ad))
                            {
                                if($row0['admin']!=$_GET['delmember'])
                                {
                                    $sql=
                                    "DELETE FROM articles 
                                     WHERE uid='$_GET[delmember]' AND gid='$_GET[n]'";
                                    mysqli_query($con, $sql);

                                    $sql=
                                    "DELETE FROM join_groups 
                                     WHERE uid='$_GET[delmember]' AND gid='$_GET[n]'";
                                    mysqli_query($con, $sql);

                                    $delfail=0;
                                }
                                else {$delfail=3;}
                            }
                         }
                }
                else
                {
                    $delfail=1;
                }
                
                unset($_GET['delmember']);
            }

             // welcome
             echo
             "<table bordercolor=white align=center width=900>
                 <tr>
                 <th bgcolor=orange colspan=3 align=left>
                 <h2><font style='background-color:orange'> &nbsp Welcome <i>".$row['name']."</i> !</font></h2>  
                     &nbsp <a href=main.php>Main Page</a>
                     &nbsp <a href=userinfo.php?n=$_SESSION[login_user]>User Info</a>
                     &nbsp <a href=logout.php?d=0>Log Out</a>
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                     &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp
                 <tr><th colspan=3><hr></th>
              <tr>
                 <th align=left>Say something...</th>
              <tr>
                 <th colspan=3 align=left>
                 <form method=get action=group.php>
                 <input type=hidden name=n value=$_GET[n]>
                 <textarea rows=6 cols=59 name='input_text'></textarea>
                 <input type='submit'>
                 </form></th>
              <tr>
              <tr>
                 <th align=left valign=top>";
                // articles
                $sql=
		"SELECT DISTINCT users.uid AS id, users.name AS user, articles.postid AS post, articles.content AS text, articles.time AS time
		 FROM            users, articles, join_groups
                 WHERE           join_groups.gid='$_GET[n]' AND users.uid=join_groups.uid AND articles.uid=join_groups.uid AND articles.gid='$_GET[n]'
                 ORDER BY        time DESC";
                
                $article=mysqli_query($con, $sql);
                
             
		while($row=mysqli_fetch_array($article))
                {
                    echo "<table bordercolor=white valign=top>
                          <tr>
                             <th align=left width=430>
                             <i>$row[user]</i> said:</th>";    
                             
                    if($row['id']==$_SESSION['login_user'])
                    {
                        echo "<th align=center>
                              <input type='button' name='delete_text' value='Delete' onclick=location.href='group.php?del=1&post=$row[post]&n=$_GET[n]'></th>";                       
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
                         "<a href=group.php?like=-1&post=$row[post]&n=$_GET[n]>Unlike</a></th>";
                    }
                    else
                    {       
                         echo
                         "<a href=group.php?like=1&post=$row[post]&n=$_GET[n]>Like</a></th>";
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
                                  <form method=get action=group.php>
                                  <input type=submit name=delete_res value=Delete>
                                  <input type=hidden name=del value=2>
                                  <input type=hidden name=delpost value=$row[post]>
                                  <input type=hidden name=delrestext value=$row0[res_text]>
                                  <input type=hidden name=n value=$_GET[n]></form></th></table>";                       
                        }
                        else
                        {
                            echo "<th width=57></th></table>";
                        }

                    }
                    echo "<table>
                          <tr>
                             <th align=left>
                             <form method=get action=group.php>
                             <textarea rows=2 cols=52 name=input_res></textarea>
                             <input type=hidden name=respost value=$row[post]>
                             <input type=hidden name=n value=$_GET[n]>
                             <input type='submit'>
                             </form></th>
                          </table>";
                    echo "<br/><br/><br/><br/>";
                }
             
            echo
            "</th>
             <th width=300></th>
             <th valign=top width=300>";
                
               $sql=
                    "SELECT gid, name, intro, admin
                     FROM   groups 
                     WHERE  groups.gid='$_GET[n]'";

                
                $groupinfo=mysqli_query($con, $sql);
	
                if($row=mysqli_fetch_array($groupinfo))
                {
                        echo 
                        "<table bordercolor=white align=left width=350>
                        <tr><th align=center colspan=2><h2>$row[name]</h2></th>
                        <tr><th align=right width=150> Intro :</th><th align=left>$row[intro]</th>
                        <tr><th align=right width=150>Administrator : </th><th align=left>$row[admin]</th>";
              
                     echo "<tr><th colspan=2 align=center><h2>Member</h2></th>";
                            
                     $sql=
                    "SELECT users.uid, users.name 
                     FROM   join_groups, users
                     WHERE  join_groups.gid='$_GET[n]'
                     AND    join_groups.uid=users.uid
                     ORDER BY name";
                
                    $member=mysqli_query($con, $sql);
                    while($row0=mysqli_fetch_array($member))
                    {
                        echo "<tr><th align=right>
                              <a href=userinfo.php?n=$row0[uid]>$row0[name]</a></th>";
                        
                        $sql0=
                             "SELECT friend_id
                              FROM   friends
                              WHERE  friends.uid='$_SESSION[login_user]'";
                                  

                             $friend=mysqli_query($con, $sql0);

                             while($row1=mysqli_fetch_array($friend))
                             {
                                    if($row0['uid']==$row1['friend_id'])
                                        echo "<th align=left><font color=red> ( friend ) </font></th>";
                             }

                             //echo "</th>";
                    }
                    
                     if($_SESSION['login_user']==$row['admin'])
                      {
                           echo
                            "
                            <tr><th colspan=2><h2> &nbsp </h2></th>
                                                 

                            <form method=get action=group.php>
                            <tr><th colspan=2><h2>Add a new member</h2></th>";
                            if(isset($addfail))
                            {
                                if($addfail==0)
                                   echo "<tr><th colspan=2><h3><font color=blue>Success!</font></h3></th>";
                                if($addfail==1)
                                   echo "<tr><th colspan=2><h3><font color=blue>The account is not existed!</font></h3></th>";
                                if($addfail==2)
                                   echo "<tr><th colspan=2><h3><font color=blue>He/She has been already in the group!</font></h3></th>";
                            }
                            echo
                            "<tr><th align=right>Member ID:  </th><th><input type=text     name='newmember'></th>
                             <input type=hidden name=n value=$_GET[n]>   
                             <tr><th colspan=2><input type=submit></th>
                             </form>";
                     
                            echo 
                            "<form method=get action=group.php>
                             <tr><th colspan=2><h2>Delete a member</h2></th>";
                            if(isset($delfail))
                            {
                                if($delfail==0)
                                   echo "<tr><th colspan=2><h3><font color=blue>Success!</font></h3></th>";
                                if($delfail==1)
                                   echo "<tr><th colspan=2><h3><font color=blue>The account is not existed!</font></h3></th>";
                                if($delfail==2)
                                   echo "<tr><th colspan=2><h3><font color=blue>He/She is not in the group!</font></h3></th>";
                                if($delfail==3)
                                   echo "<tr><th colspan=2><h3><font color=blue>Can not delete the administor!</font></h3></th>";
                            }
                     
                            echo
                           "<tr><th align=right>Member ID:  </th><th><input type=text     name='delmember'></th>                   
                            <tr><th colspan=2><input type=submit></th>
                            <input type=hidden name=n value=$_GET[n]>
                            </form></table></th></table>";
                     }
                }
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