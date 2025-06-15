# Conflicts
Please resolve them and commit them using the commands `Git: Commit all changes` followed by `Git: Push`
(This file will automatically be deleted before commit)
[[#Additional Instructions]] available below file list

- Not a file: incl/Server.hpp
- Not a file: src/ConfigFile.cpp
- Not a file: src/Server.cpp
- Not a file: src/ServerLoop.cpp
- Not a file: src/ServerSocket.cpp
- Not a file: www/config/server.conf

# Additional Instructions
I strongly recommend to use "Source mode" for viewing the conflicted files. For simple conflicts, in each file listed above replace every occurrence of the following text blocks with the desired text.

```diff
<<<<<<< HEAD
    File changes in local repository
=======
    File changes in remote repository
>>>>>>> origin/main
```