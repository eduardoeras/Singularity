package extractor.tools;

import global.structure.Transition;

import java.util.List;

public class TransitionPrinter {
    //Attributes

    //Constructor
    public TransitionPrinter () {

    }

    //Methods
    public void print (List<Transition> transitions) {
        for (Transition transition : transitions) {
            System.out.print("<transition from = '");
            System.out.print(transition.getFrom().getLabel());
            System.out.print("' to = '");
            System.out.print(transition.getTo().getLabel());
            System.out.print("' event = '");
            System.out.print(transition.getEvent().getEvent());
            System.out.println("'>");
        }
    }

}
