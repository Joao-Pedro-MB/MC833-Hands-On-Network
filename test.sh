sudo apt-get update
sudo apt-get install mysql-server
sudo systemctl start mysql
sudo apt-get install libmysqlclient-dev
mysql -u root
#gcc -o main main.c -lmysqlclient