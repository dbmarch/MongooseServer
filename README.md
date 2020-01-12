# MongooseServer


### Dependencies
This project includes a few 3rd party open source packages:
- jsoncpp
- mongoose

This project has a submodule with a React App Client:
- client

### BUILDING

```
    git clone https://github.com/dbmarch/MongooseServer.git

    cd MongooseServer

    git submodule update --init

    make
```

### RUN SERVER

To start the server:

```
  bin/MongooseServer
```

    Browse localhost:8000


### BUILDING CLIENT

To build a new product file set:
```
  cd client
  npm i
  npm run build
```


To run the dev server:
To build a new product file set:
```
  cd client
  npm i
  npm run start
```