package extractor.transition;

import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class TransitionFilter {
    //Attributes

    //Constructor

    //Methods
    public void filter (List<Transition> transitions) {
        List<Transition> rejected = new ArrayList<>();
        for (Transition transition : transitions) {
            if (transition.getEvent().getEvent().equals("true")) {
                transition.getEvent().setEvent("TRUE");
            } else if (transition.getEvent().getEvent().equals("false")) {
                transition.getEvent().setEvent("FALSE");
            }
            if (transition.getFrom().getLabel().equals("else") & transition.getEvent().getEvent().equals("FALSE")) {
                rejected.add(transition);
            }
        }
        for (Transition transition : rejected) {
            transitions.remove(transition);
        }
    }

}
