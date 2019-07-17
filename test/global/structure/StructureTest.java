package global.structure;

import global.tools.IdGenerator;
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
        IdGenerator.reset();

        Instruction origin = new Level();
        Instruction destiny = new State();
        Transition transition = new Transition();

        Event event = new Event();
        event.setEvent("Boom!");

        transition.setEvent(event);
        transition.setFrom(origin);
        transition.setTo(destiny);

        System.out.println("\n- - - - - - - - - - - - - - - - - - - - -");
        System.out.println("Transition origin: " + transition.getFrom().getId());
        System.out.println("Transition destiny: " + transition.getTo().getId());
        System.out.println("Transition event: " + transition.getEvent().getEvent());
        System.out.println("- - - - - - - - - - - - - - - - - - - - -");

        IdGenerator.reset();

        System.out.println("\n- - - - - - - - - - - - - - - - - - - - -");
        System.out.println("Transition origin: " + transition.getFrom().getId());
        System.out.println("Transition destiny: " + transition.getTo().getId());
        System.out.println("Transition event: " + transition.getEvent().getEvent());
        System.out.println("- - - - - - - - - - - - - - - - - - - - -");
    }

}
