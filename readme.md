### m_json

An easy to use JSON library written in c++. m_json can parse JSON as well as write it using simple intuitive syntax.

Note: Work in Progress!

###Features
* Parse JSON from a file or a string
* Write JSON to existing JSON or save to a string/file
* Use simple, intuitve syntax to read and write your JSON
*


###Example

example_file.json
```
{
	"userId": 4,
	"id": 1,
	"title": "this is a great title",
	"completed": false,
	"test_bool": true,
	"test_obj": {
		"blah": 5
	},
	"test_array": [ "blah", "foo", "what" ]
}
```

load the JSON from file
```
std::string str = load_json_from_file("example_test.json");
Json json = parse(str);
```

use the JSON values 
```
int user_id = json["userId"].to_int(); // user_id => 4
std::string title = json["title"].to_str(); // title => "this is a great title"

std::string arr_val = json["test_array"].to_array()[1]; // arr_val => "foo"
```
