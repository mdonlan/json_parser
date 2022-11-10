A JSON parsing library written in c++.

Note: Work in Progress!

### Features
* Parse JSON from a file or a string
* Write JSON to existing JSON or save to a string/file
* Use simple, intuitve syntax to read and write your JSON


### Example

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
Json_Value json = parse(load_json_from_file("example_test.json"));
```

use the JSON values 
```
int user_id = json["userId"].to_int(); // user_id == 4
bool completed = json["completed"].to_bool(); // completed == false
std::string title = json["title"].to_str(); // title == "this is a great title"
std::string arr_val = json["test_array"].to_array()[1].to_string(); // arr_val == "foo"
```
