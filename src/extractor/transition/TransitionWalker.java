package extractor.transition;

import global.structure.Element;
import global.structure.State;
import global.structure.Transition;
import global.structure.Visibility;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class TransitionWalker {
    //Attributes
    private StringTools stringTools;

    //Constructor
    public TransitionWalker () {
        stringTools = new StringTools();
    }

    //Public Methods
    public void walk (List<State> states, List<Transition> transitions) {
        createMainFunction(states);
    }

    //Private Methods
    private void createMainFunction (List<State> states) {
        List<State> main = new ArrayList<>();

        for (State state : states) {
            if (state.getElement() == Element.CLASS || state.getElement() == Element.STRUCT) {
                System.out.println("CLASS/SCOPE");
                System.out.println(state.getLabel());
            }
            if (state.getElement() == Element.FUNCTION) {
                System.out.println("FUNCTION");
                System.out.println(state.getLabel());
            }
        }
    }

}
