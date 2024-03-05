select * from t_user_file;
select * from t_file;

select f_id,f_name,f_size,f_uploadtime,f_type from user_file_info where u_id=1 and f_dir='' and f_state=1;
select f_id,f_name,f_size,f_uploadtime,f_type from user_file_info where u_id=1 and f_dir='/新建/' and f_state=1;