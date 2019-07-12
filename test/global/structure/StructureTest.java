package global.structure;

import org.junit.Before;
import org.junit.Test;

public class StructureTest {
    //Attributes
    private Instruction state;
    private Instruction level;
    private Instruction transition;

    //Constructor
    @Before
    public void init () {
        state = new State();
        level = new Level();
        transition = new Transition();
    }

    //Methods
    @Test
    public void test () {

    }

}
