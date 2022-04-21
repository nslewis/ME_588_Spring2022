# ME 588 Spring 2022 Final Project
## Whack-A-Mole Game Description

In a game of Robotic Whack-A-Mole, you will be placing Mole-Whackers (foam balls or cubes) on
squares with Moles. The squares that Moles are occupying will be identified by color, with the field
having 3 different color squares (not including the starting square which will be white). At the start of
each game, a TA will tell you the color of the squares where Moles are hiding. Your robot must then
autonomously search the playing field for the Moles and place Mole-Whackers on those squares.

## Playing Field

![alt text](https://github.com/nslewis/ME_588_Spring2022/blob/main/Images/playing_field.png)
The playing field is a square plywood surface (8 ft. x 8 ft.) that has been separated into 16 equal sized
squares. Side rails which are made of white foam board will be placed around the perimeter of the
field to prevent robots from rolling off the field, shown as bold solid gray lines in Fig. 1. These rails
extend vertically upwards 6 inches beyond the plywood surface, but may not be perfectly
perpendicular to horizontal plywood surface. The playing field is divided into colored squares that are
2 ft. x 2 ft by black lines. These lines are physically created by using 3/4 inches width black electrical
tape. The starting square is white, while the remaining 15 squares are red, green, or blue, with 5 of
each color. Detailed dimensions of the playing field are shown above in Figure 1. The color distribution shown
may not be the final one for the competition.

## Installation

Use FSM_Final.ino to run the robots. LineTestingTurning.ino helps debug turning when following the line. 

## Usage

Just upload the necessary code to the Arduino Board. Here an Arduino Mega Board was used. 

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)
