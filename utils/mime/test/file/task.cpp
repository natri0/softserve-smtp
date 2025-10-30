#include <iostream>
#include <vector>
#include <random>
#include <time.h>

/*
You are given a locked container represented as a two-dimensional grid of boolean values (true = locked, false = unlocked). 
Your task is to write an algorithm that fully unlocks the box, i.e., 
transforms the entire matrix into all false.

Implement the function:
bool openBox(uint32_t y, uint32_t x);
This function should:
    - Use the SecureBox public API (toggle, isLocked, getState).
    - Strategically toggle cells to reach a state where all elements are false.
    - Return true if the box remains locked, false if successfully unlocked.
You are not allowed to path or modify the SecureBox class.

Evaluation Criteria:
    - Functional correctness
    - Computational efficiency
    - Code quality, structure, and comments
    - Algorithmic insight and clarity
*/

class SecureBox
{
private:
    std::vector<std::vector<bool>> box;

public:

    //================================================================================
    // Constructor: SecureBox
    // Description: Initializes the secure box with a given size and 
    //              shuffles its state using a pseudo-random number generator 
    //              seeded with current time.
    //================================================================================
    SecureBox(uint32_t y, uint32_t x): ySize(y), xSize(x)
    {
        rng.seed(time(0));
        box.resize(y);
        for (auto& it : box)
            it.resize(x);
        shuffle();
    }

    //================================================================================
    // Method: toggle
    // Description: Toggles the state at position (x, y) and also all cells in the
    //              same row above and the same column to the left of it.
    //================================================================================
    void toggle(uint32_t y, uint32_t x)
    {
        box[y][x] = !box[y][x];
        for (uint32_t i = 0; i < xSize; i++)
            box[y][i] = !box[y][i];
        for (uint32_t i = 0; i < ySize; i++)
            box[i][x] = !box[i][x];
    }

    //================================================================================
    // Method: isLocked
    // Description: Returns true if any cell 
    //              in the box is true (locked); false otherwise.
    //================================================================================
    bool isLocked()
    {
        for (uint32_t x = 0; x < xSize; x++)
            for (uint32_t y = 0; y < ySize; y++)
                if (box[y][x])
                    return true;

        return false;
    }

    //================================================================================
    // Method: getState
    // Description: Returns a copy of the current state of the box.
    //================================================================================
    std::vector<std::vector<bool>> getState()
    {
        return box;
    }

private:
    std::mt19937_64 rng;
    uint32_t ySize, xSize;

    //================================================================================
    // Method: shuffle
    // Description: Randomly toggles cells in the box to 
    // create an initial locked state.
    //================================================================================
    void shuffle()
    {
        for (uint32_t t = rng() % 1000; t > 0; t--)
            toggle(rng() % ySize, rng() % xSize);
    }
};

//================================================================================
// Function: openBox
// Description: Your task is to implement this function to unlock the SecureBox.
//              Use only the public methods of SecureBox (toggle, getState, isLocked).
//              You must determine the correct sequence of toggle operations to make
//              all values in the box 'false'. The function should return false if
//              the box is successfully unlocked, or true if any cell remains locked.
//================================================================================
bool openBox(uint32_t y, uint32_t x)
{
    SecureBox box(y, x);

    // recive start state
    auto initialState = box.getState();
    
    // I implement the Gf(2) algorithm
    // I use a vector with only 0 and 1, where 1 means the cell will be toggled
    std::vector<std::vector<bool>> toggleMatrix(y * x, std::vector<bool>(y * x + 1, false));
    
    // create a matrix of coefficients
    for (uint32_t i = 0; i < y; i++) {
        for (uint32_t j = 0; j < x; j++) {
            // index 
            int eq = i * x + j;
            
            // Set the current state of the cell
            toggleMatrix[eq][y * x] = initialState[i][j];
            
            // Fill in the coefficients for toggle each cell
            for (uint32_t ti = 0; ti < y; ti++) {
                for (uint32_t tj = 0; tj < x; tj++) {
                    int var = ti * x + tj;
                    
                    // Effect of toggle on positions (ti, tj):
                    bool affects = false;
                    
                    // Check if this is the same cell (inverts three times -> 1 time)
                    if (ti == i && tj == j) {
                        affects = true;
                    }
                    // Check if the cell is in the same row
                    else if (ti == i) {
                        affects = true;
                    }
                    // Check if the cell is in the same column
                    else if (tj == j) {
                        affects = true;
                    }
                    
                    toggleMatrix[eq][var] = affects;
                }
            }
        }
    }
    
    // Straight line GF(2)
    for (uint32_t i = 0; i < y * x; i++) {
        // find first 1 in column
        uint32_t pivot_row = i;
        while (pivot_row < y * x && !toggleMatrix[pivot_row][i]) {
            pivot_row++;
        }
        
        // If there is no one in this column, go to the next one
        if (pivot_row == y * x) {
            continue;
        }
        
        // excange row
        if (pivot_row != i) {
            toggleMatrix[i].swap(toggleMatrix[pivot_row]);
        }
        
        // Subtract the current row from all lower rows of 1 in this column
        for (uint32_t j = i + 1; j < y * x; j++) {
            if (toggleMatrix[j][i]) {
                for (uint32_t k = i; k <= y * x; k++) {
                    toggleMatrix[j][k] = toggleMatrix[j][k] != toggleMatrix[i][k]; // XOR in GF(2)
                }
            }
        }
    }
    
    // Backtracking (finding a solution)
    std::vector<bool> solution(y * x, false);
    for (int i = y * x - 1; i >= 0; i--) {
        bool sum = toggleMatrix[i][y * x];
        for (uint32_t j = i + 1; j < y * x; j++) {
            if (toggleMatrix[i][j] && solution[j]) {
                sum = !sum; // XOR in GF(2)
            }
        }
        solution[i] = sum && toggleMatrix[i][i]; // if the main element is 0, the variable can be anything
    }
    
    // Apply the found solution
    for (uint32_t i = 0; i < y; i++) {
        for (uint32_t j = 0; j < x; j++) {
            if (solution[i * x + j]) {
                box.toggle(i, j);
            }
        }
    }

    return box.isLocked();
}


int main(int argc, char* argv[])
{
    uint32_t y = std::atol(argv[1]);
    uint32_t x = std::atol(argv[2]);
    bool state = openBox(y, x);

    if (state)
        std::cout << "BOX: LOCKED!" << std::endl;
    else
        std::cout << "BOX: OPENED!" << std::endl;

    return state; 
}

