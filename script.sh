#Store array of arguments into an array.
args=("$@") 
#Check for exactly 2 arguments.
if [ "${#args[@]}" != "2" ]; then
    printf "You have to insert exactly these 2 arguments...\nNumber_Of_Consumers Size_Of_Array\n"; exit 1
fi
make
./feeder $1 $2