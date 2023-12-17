#用户表中密码采用MD5加密存储
DROP TABLE IF EXISTS t_user ;
CREATE TABLE t_user (
u_id bigint(11) NOT NULL AUTO_INCREMENT, 
u_tel varchar(20) NOT NULL ,
u_password varchar(45) NOT NULL, 
u_name varchar(45) NOT NULL, 
PRIMARY KEY ( u_id )
)ENGINE=InnoDB CHARSET=utf8;
#文件信息表 t_file
DROP TABLE IF EXISTS t_file ;
CREATE TABLE t_file (
f_id bigint(11) NOT NULL AUTO_INCREMENT, 
f_size bigint(11) NOT NULL, 
f_path varchar(260) NOT NULL, 
f_count bigint(11) NOT NULL, 
f_MD5 varchar(45) NOT NULL, 
f_state int NOT NULL , 
f_type varchar( 10 ) NOT NULL,
PRIMARY KEY ( f_id )
) ENGINE=InnoDB CHARSET=utf8;
#用户文件表 t_user_file ( 用户 id , 文件 id , 目录)
DROP TABLE IF EXISTS t_user_file ;
CREATE TABLE t_user_file ( 
u_id bigint(11) NOT NULL, 
f_id bigint(11) NOT NULL, 
f_dir varchar(260) NOT NULL ,
f_name varchar(260) NOT NULL ,
f_uploadtime datetime NOT NULL,
s_link bigint(11) ,#分享的链接
s_linkPassword bigint(4) ,#链接密码
s_linkTime datetime #分享时间
) ENGINE=InnoDB CHARSET=utf8;
#上传表 t_upload
create table t_upload (
f_id int,
f_name varchar(260),
f_dir varchar(260),
f_absolutePath varchar(260),#绝对路径
f_size int,
f_md5 varchar(40),
f_time varchar(40),
f_type varchar(10),
f_timestamp int 
);
#下载表 t_download
create table t_download (
f_id int,
f_name varchar(260),
f_dir varchar(260),
f_absolutePath varchar(260),
f_size int,
f_md5 varchar(40),
f_time varchar(40),
f_type varchar(10) ,#文件标志,例如"文件" "文件夹" 等
f_timestamp int
);