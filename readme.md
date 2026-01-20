# Astonia 3 Server, Community Editon

## Warning
Do not deploy on an existing database! At the very least the hashed password
will prevent anyone from logging in and there might be more changes to the
table structure eventually.

## Building

### Ubuntu
To run under Ubuntu 22.04.4 LTS (tested and working as of 2024.06.27)

```
sudo apt-get update
sudo apt install git
git clone https://github.com/AstoniaCommunity/astonia_community_server3.git
sudo apt-get install gcc-multilib
sudo dpkg --add-architecture i386
sudo apt-get update # yes, we need to run this again to get the i386 packages
sudo apt install make
sudo apt install lib32z-dev libargon2-dev:i386
sudo apt install libmysqlclient-dev:i386
sudo apt install mariadb-server
cat MYSQLPASSWD
sudo mysql_secure_installation
# old root password is blank. new root password from 'cat MYSQLPASSWD'.
# do not switch to unix sockets
./my <create_tables.sql 
./my merc <merc.sql 
./my merc <storage.sql 
echo "Welcome to Astonia" >motd.txt
rm MYSQLPASSWD
# the "my" script is very handy, but also a security risk!
make -j 4
./server
# look for errors, then hit CTRL-C to stop
./start # will start all areas and the chatserver
./create_account <email> <password>
./create_character 1 Ishtar MWG
```

Now you should be able to connect to the server with a client, using
something like:

```
bin\moac -uIshtar -p<password> -d<server IP or name> -v35
```

Finally, you should setup your firewall. This isn't technically part of this
guide, but it might be... wise.
```
sudo ufw allow 22/tcp
sudo ufw deny 5554/tcp
sudo ufw deny 3306/tcp
sudo ufw allow 8080:8090/tcp
sudo ufw allow 27584:27777/tcp
sudo ufw enable
```

If you want to be able to run "make pretty":
```
sudo apt-get install -y wget gnupg lsb-release software-properties-common
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/llvm.asc
sudo add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-21 main"
sudo apt-get update
sudo apt install clang-format-21
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-21 200
sudo update-alternatives --set clang-format /usr/bin/clang-format-21
clang-format --version
```

The last step should output a version starting with 21. now.
