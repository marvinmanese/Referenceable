# Referenceable
Referenceable class used for safe referencing of an object.  

  
### Example usage:  
// Map of referenceable integers  
std::unordered_map<std::string, mmfutils::referenceable<int>>; integer_map;  
integer_map.emplace("first_item", 1);  
integer_map.emplace("second_item", 2);  
integer_map.emplace("third_item", 3);  
integer_map.emplace("fourth_item", 4);  


// Reference the third item  
mmfutils::reference<int> second_item = integer_map.at("second_item");  
mmfutils::reference<int> third_item = integer_map.at("third_item");  
if(second_item && third_item) {  
&emsp;std::cout << "References are valid!" << std::endl;  
&emsp;std::cout << "Second item: " << *second_item.get() << std::endl;  
&emsp;std::cout<< "Third item: " << *third_item.get() << std::endl;  
}  


// Remove the third item  
std::cout << std::endl << "Removing third item..." << std::endl << std::endl;  
integer_map.erase("third_item");  


// Try to access second and third item reference again  
std::cout << "Items that have expired: " << std::endl;  
if(second_item.expired()) {  
&emsp;std::cout << "-Second item" << std::endl;  
}  
if(third_item.expired()) {  
&emsp;std::cout << "-Third item" << std::endl;  
}
