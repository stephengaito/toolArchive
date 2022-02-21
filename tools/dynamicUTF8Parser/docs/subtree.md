# Using git subtree

We use git subtree to include a copy of Daniel C. Jones's 
[HAT-Trie](https://github.com/dcjones/hat-trie) implementation.

We base our use of git subtree on Stu Campbell's [Using Git subtrees 
for repository 
separation](https://makingsoftware.wordpress.com/2013/02/16/using-git-subtrees-for-repository-separation/) 
tutorial.

Specifically we used the commands:

    git remote add hat-trie https://github.com/dcjones/hat-trie
    git subtree add --prefix=HAT-trie --squash hat-trie master

To copy the hat-trie code into this project.
