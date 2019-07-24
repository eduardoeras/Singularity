package extractor;

import extractor.state.StateWalker;
import extractor.transition.TransitionWalker;
import global.structure.Instruction;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Extractor {
    //Attributes
    private StateWalker stateWalker;
    private TransitionWalker transitionWalker;
    //Constructor
    public Extractor () {
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
    }

    //Methods
    public void extract (ParseTree parseTree) {
        List<Instruction> states = new ArrayList<>();
        stateWalker.walk(parseTree, states);
        List<Transition> transitions = new ArrayList<>();
        transitionWalker.walk(parseTree, transitions);
    }

}
