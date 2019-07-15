package global.structure;

import org.junit.Before;
import org.junit.Test;

public class StructureTest {
    //Attributes

    //Constructor
    @Before
    public void init () {

    }

    //Methods
    @Test
    public void test () {
        Instruction origin = new Level();
        Instruction destiny = new State();
        Transition transition = new Transition();

        Event firstEvent = new Event();
        firstEvent.setEvent("Boom!");

        transition.setEvent(firstEvent);
        transition.setFrom(origin);
        transition.setTo(destiny);

        System.out.println("- - - - - - - - - - - - - - - - - - - - -");
        System.out.println("Transition origin: " + transition.getFrom());
        System.out.println("Transition destiny: " + transition.getTo());
        System.out.println("Transition event: " + transition.getEvent().getEvent());
        System.out.println("- - - - - - - - - - - - - - - - - - - - -");
    }

}
