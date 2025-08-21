Play button is pressed

Floor number top middle

BACK button on the top left
INVENTORY button on the top right

Black and white 5x5 black squares with white borders, 
connected by white lines.

smaller green box to indicate player position

Text box at the bottom.
{Inside the text box}

This room is clear. Where will you go? 
(buttons)
North  East
South  West 
 
{If a direction where a boundary is pressed}
A wall stops your path. 
(buttons)
Continue

{If the movement goes to a valid room}

    {If it is a treasure room}
        You sense a radiant presence beyond this door. Continue?
        (Buttons)
        Yes No 

        {if yes}
        A chest lays in the center of the room. You open it and recieve [ITEM].
        (buttons)
        Continue

        {Loot pool is based on the floor number}
        
        {Back to WHERE WILL YOU GO prompt}

    {If it is the boss room}
        You sense an ominous presence beyond this door. Continue?
        (buttons)
        Yes No 

        {If no, back to WHERE WILL YOU GO prompt}
        {If yes, fade to black and update position}
    
    {If it is a regular enemy room}
        You sense a dangerous presence beyond this door. Continue?
        (buttons)
        Yes No 

        {If no, back to WHERE WILL YOU GO prompt}
        {If yes, fade to black and update position}


-
on init
x_pos = 
y_pos = 
-

if identify(x_pos, y_pos) == clear 
    text = 
    state = comment_state



(Only activate on the first pass)
if state_change = true:
    text_set = 
    button[0] = 
    button[1] = 
    button[2] = 
    button[3] = 

    proposed_x_pos = x_pos
    proposed_y_pos = y_pos

if button 0 pressed 
    state_change = true 
    identify 


if button 1 pressed 
    state_change = True 

if button 2 pressed
    state_change = True 

if button 3 pressed 
    state_change = True 

if state_Change = true
    identify(porposedx,porposedy)
    if id == None:
        text = 
        state = comment_state

        
    if id == boss:
        text = 
        state = confirm_state

    if id == 
        text = 
        state = confirm_state
    
    if id == 
        text = 
        state = confirm_state

    if id == 
        text =
        state = confirm_state


Move_commit state