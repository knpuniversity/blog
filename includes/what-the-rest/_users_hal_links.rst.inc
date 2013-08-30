.. code-block:: json

    {
      "_links": {
        "self": {
          "href": "http://api.example.com/users",
          "title": "Users in the system"
        },
        "http://api.example.com/rels/users_reinvite": {
          "href": "http://api.example.com/users",
          "title": "Re-invite unregistered users to the system"
        },
        
        "...": "... other links, like first, prev, next, last ..."
      },
      "...": "... other stuff, like embedded users ..."
    }