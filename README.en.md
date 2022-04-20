# content

1. Project requirements and goals
2. Development environment
3. Introduction to JSON
4. muduo network library programming
5. Server cluster
6. Redis based on publish-subscribe - server middleware
7. Database Design

The technology stack to be used in this project: 
1. Json serialization and deserialization; 
2. muduo network library development; 
3. nginx source code compilation and installation and environment deployment; 
4. nginx's tcp load balancer configuration; 
5. redis cache Server programming practice; 
6. Programming practice of server middleware redis message queue based on publish-subscribe; 
7. MySQL database programming; 
8. CMake build environment; 
9. Github hosting project

The content of this project includes: commonly developed server, network, business, data modules (database, data operation), the three major modules should be distinguished in the project, and the login module is the main line at the beginning of the project, which is divided into three major blocks.

# Project requirements and goals

* Project requirements
  1. Client new user registration
  2. Client user login
  3. Add friends and add groups
  4. Friend chat and group chat
  5. nginx configure tcp load balancing
  6. The cluster chat system supports client-side cross-server communication

* Project Objectives
  1. Master the design idea of ​​the server's network I/O module, business module and data module layering
  2. Master the programming and implementation principles of the C++ muduo network library
  3. Master the programming application of Json
  4. Master the principle and application of nginx configuration and deployment of tcp load balancer
  5. Master the application scenarios of server middleware and redis programming practice and application principles based on publish-subscribe
  6. Master CMake to build an automated programming environment

# development environment

The specific configuration is omitted, read other articles.

## project directory

```
The "include" directory is where the header files are placed.
The code of the server and client are in the same project, and the C/S can be generated separately in the "bin" directory during the final generation.
It can be classified by "server" and "client". For example, the header files needed to generate code can be placed in "/include/server" and "/include/client" respectively, and the header files required by server and client can be placed directly under "/include". such as "message id".
"src" stores open source.
"thirdparty" is the third-party library folder, such as "json.hpp".
This project does not generate "lib" library (.a/.so), so there is no lib folder.
```

# Json introduction

Json is a lightweight data interchange format (also called data serialization). Json in a completely **programming language independent** text format
to store and represent data. Concise and clear hierarchy makes Json an ideal data exchange language. It is easy for humans to read and write, but also easy for machines to parse and generate, and effectively improve the efficiency of network transmission.

* Json third-party library

This project uses JSON for Modern C++, a JSON library written by German nlohmann under C++.

Features: 
1. The entire code is contained by a header file json.hpp, which has no dependencies and is easy to use; 
2. It is written using the C++11 standard; 
3. It makes json like an STL container, and the STL and json containers can communicate with each other. Conversion; 
4. All classes are strictly unit tested, covering 100% of the code, including all special behaviors. Also, Valgrind is checked for memory leaks. In order to maintain high quality, the project follows the best practices of the "Core Infrastructure" initiative.

# Reactor model

This project is based on the muduo library. The model is based on an event-driven, IO multiplexing + epoll + thread pool network. It is completely based on the Reactor model. The number of threads is temporarily set to 4. One main Reactor is the IO thread, which is mainly responsible for the connection of new users. The three sub-Reactors are worker threads, which are mainly responsible for the processing of read and write events of connected users.

# data module

## ORM framework

Object Relation Map - Object Relational Map.

In this framework, all operations in the business layer are objects, and no specific SQL operations can be seen.

In the DAO layer (data layer), there are specific database operations.

Solved the pain point: decoupling between business modules and data modules.