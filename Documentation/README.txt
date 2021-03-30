Please note, the Netbeans project in this folder is effectively a place-holder,
this allows for the loading of the linked projects in the sub-folders via the
'related projects' project-group. It also allows for in-IDE SVN-commit/update.

/*
    // normal array
    GLfloat normals[] = {0,0,1,  0,0,1,  0,0,1,  0,0,1,             // v0-v1-v2-v3
                         1,0,0,  1,0,0,  1,0,0, 1,0,0,              // v0-v3-v4-v5
                         0,1,0,  0,1,0,  0,1,0, 0,1,0,              // v0-v5-v6-v1
                         -1,0,0,  -1,0,0, -1,0,0,  -1,0,0,          // v1-v6-v7-v2
                         0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,         // v7-v4-v3-v2
                         0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1};        // v4-v7-v6-v5

    // color array
    GLfloat colors[] = {1,1,1,  1,1,0,  1,0,0,  1,0,1,              // v0-v1-v2-v3
                        1,1,1,  1,0,1,  0,0,1,  0,1,1,              // v0-v3-v4-v5
                        1,1,1,  0,1,1,  0,1,0,  1,1,0,              // v0-v5-v6-v1
                        1,1,0,  0,1,0,  0,0,0,  1,0,0,              // v1-v6-v7-v2
                        0,0,0,  0,0,1,  1,0,1,  1,0,0,              // v7-v4-v3-v2
                        0,0,1,  0,0,0,  0,1,0,  0,1,1};             // v4-v7-v6-v5

    // index array of vertex array for glDrawElements()
    // Notice the indices are listed straight from beginning to end as exactly
    // same order of vertex array without hopping, because of different normals at
    // a shared vertex. For this case, glDrawArrays() and glDrawElements() have no
    // difference.
    GLubyte indices[] = {0,1,2,3,
                         4,5,6,7,
                         8,9,10,11,
                         12,13,14,15,
                         16,17,18,19,
                         20,21,22,23};*/