#注册 , 登录 
#用户信息表
#t_user( u_id , u_tel , u_password , u_name )
#文件表 文件详细信息
#t_file ( f_id , f_size , f_path , f_MD5 , f_count , f_state , f_type )
#用户文件关系表
#t_user_file ( u_id , f_id , f_dir , f_name , f_uploadtime , s_link , s_linkPassword , s_linkTime )
#1.联表查询 t_file 和 t_user_file 的视图
#新建视图 user_file_info
DROP view IF EXISTS user_file_info ;
create view user_file_info as (
select  f_size , f_MD5 , f_type , f_path , f_count , f_state , t_user_file.* 
from t_file inner join t_user_file on t_file.f_id = t_user_file.f_id 
);
#2. 插入数据到用户表 用于完成注册登录
insert into t_user ( u_tel , u_password , u_name ) values( '13312341234' , 'abc12345…' , '张三');
#password 是 md5
#比如这个 张三 id 是 1
#查询语句
select count(*) from t_user where u_tel = '%s' and u_password = '%s';
select * from t_user where u_tel = '13312341234' and u_password = 'abc12345…';
#3.用户 id 为 1 的人, 上传一个文件 abc.txt, 在默认路径下 ( 用户和文件建立关系, 需要使用触发器给引用计数+1) 上传时间比如是 2023-04-18 20:26:12
#步骤 1: 创建这个文件信息 引用计数是 0 ,比如 abc.txt 大小是 301 字节 , md5 是 13523afe…
insert into t_file ( f_size , f_path , f_MD5 , f_count , f_state , f_type ) values ( 301 , '/home/colin/NetDisk/1/abc_md5.txt' , '13523afe…' ,0 , 1 , 'file');
#比如插入的是 f_id 为 1 
#步骤 2 添加到用户文件关系表中
insert into t_user_file (u_id , f_id , f_dir , f_name , f_uploadTime ) values ( 1 , 1 , '/' , 'abc.txt' , '2023-04-18 20:26:12');
#步骤 3 触发器 对 f_count 自动+1
#4. 用户 id = 1 的人获取所有文件信息(完成上传的) 用于登录之后, 获取列表 , 获取根目录的文件列表('/')
select * from user_file_info where u_id = 1 and f_dir = '/' and f_state = 1;
#不需要都查看 只需要 f_name f_size f_uploadTime f_type f_dir
#以下为查询某人 某目录下 完成上传的文件
#select f_name,f_size,f_uploadTime, f_type, f_dir from user_file_info where u_id = %d and f_dir = '%s' and f_state = 1;
select f_name,f_size,f_uploadTime, f_type, f_dir from user_file_info where u_id = 1 and f_dir = '/' and f_state = 1;
#5. 用户 id =1 , 在'/' 目录下面, 新建文件夹 0418
#新建的文件夹也是文件 , 和上面类似
insert into t_file ( f_size , f_path , f_MD5 , f_count , f_state , f_type ) values ( 0 , '/home/colin/NetDisk/1/ ' ,'...' , 0 , 1 , 'folder' );
#f_id 比如是 2
insert into t_user_file (u_id , f_id , f_dir , f_name , f_uploadTime ) values ( 1 , 2 , '/' , '0418' , '2023-04-18 20:38:12');
#触发器 会对 f_count +1
#6. 用户 u_id = 2 秒传 文件 abc.txt 到根目录
#判断 md5 是否一致
#select f_id from user_file_info where f_MD5 = '%s';
select f_id from user_file_info where f_MD5 = '13523afe…' ;
insert into t_user_file (u_id , f_id , f_dir , f_name , f_uploadTime ) values ( 2 , 1 , '/' , 'abc.txt' , '2023-04-18 20:42:12');
#7.0 分享某文件 分享用户 id=1 的, 文件 id=1 , 路径为'/'的文件 比如分享码是 12345678
update t_user_file set s_link = '12345678' , s_linkTime = '2023-04-19 20:42:12' where u_id = 1 and f_id = 1 and f_dir = '/';
#7.1 用户 u_id =1 的人获取自己的所有分享的文件
select f_name , f_size , s_linkTime , s_link from user_file_info where u_id = 1 and s_link is not null;
select * from user_file_info;
select * from t_user_file;
#8. 用户 u_id =2 添加分享到自己根目录下
    #加入t_user_file表中,需要的信息:u_id,f_id,f_dir,f_name...
    insert into t_user_file (u_id,f_id,f_dir....)
    select u_id,f_id,f_dir....
    from user_file_info where s_link='';
#首先 获取现在的时间 比如是 '2023-04-19 21:00:00' 然后分享码 是 '12345678' , 用户 id=2
#然后获得对应文件信息 从视图中获取
select f_id , f_name from user_file_info where s_link = '12345678' and u_id = 2;
#添加文件信息到自己目录下
#比如这个人是 u_id = 3;根目录是 '/' , f_id 比如是 1 f_name 比如是 'abc.txt'
insert into t_user_file( u_id , f_id , f_dir , f_name , f_uploadtime ) values( 3 , 1 , '/' , 'abc.txt' , '2023-04-19 21:00:00');
#因为又触发器, 那么当插入用户文件关系时, 文件引用计数会自动+1
#触发器 -- 添加用户文件关系 以及删除 用户文件关系 文件引用计数 +1 -1
#编写触发器
#需求:
#插入用户文件关系的时候, 对对应的 id 的文件信息, 文件引用+1;
#插入语句:
insert into t_user_file ( u_id , f_id , f_dir , f_name , f_uploadtime ) values( %d , %d , '%s' , '%s' , '%s' );
 #参数 分别是 u_id f_id f_dir , f_name , f_uploadtime
drop trigger if exists refPlusOne; 
delimiter // 
create trigger refPlusOne
after insert 
on t_user_file 
for each row 
begin 
update t_file set f_count = f_count + 1 where t_file.f_id = new.f_id; 
end // 
delimiter ;
#编写触发器
#需求:
#1.当删除 t_user_file 的表中数据时, 比如 u_id 的 f_dir 路径的 f_id 文件, 删除该条记录, #那么 t_file 对应的文件的引用计数-1
#2.如果引用计数达到 0 , 那么删除 t_file 表中该条数据. 
#那么需要两个触发器.
#写好触发器之后, 服务端删除关联关系, 在 t_file 表中会自动改变引用计数.
#答:
#删除语句
delete from t_user_file where u_id = %d and f_id = %d and f_dir = '%s' ;
drop trigger if exists refMinusOne;
delimiter // 
create trigger refMinusOne
after delete 
on t_user_file 
for each row 
begin 
update t_file set f_count = f_count - 1 where t_file.f_id = old.f_id; 
delete from t_file where f_count = 0 and t_file.f_id = old.f_id; 
end // 
delimiter ;