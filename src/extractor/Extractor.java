package extractor;

//import extractor.tools.StatePrinter;
import extractor.tools.TransitionPrinter;
import global.structure.Line;
import extractor.state.StateWalker;
import extractor.state.Matrix;
import extractor.transition.TransitionWalker;
import global.structure.State;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Extractor {
    //Attributes
    private Matrix matrix;
    private StateWalker stateWalker;
    private TransitionWalker transitionWalker;
    //private StatePrinter statePrinter;

    //Constructor
    public Extractor () {
        matrix = new Matrix();
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
        //statePrinter = StatePrinter.getInstance();
    }

    //Public Methods
    public void extract (ParseTree parseTree) {
        List<State> states = new ArrayList<>();
        List<Transition> transitions = new ArrayList<>();
        List<Line> cppMatrix = new ArrayList<>();
        cppMatrix.add(new Line());

        matrix.matrixGenerator(cppMatrix, parseTree);
        stateWalker.walk(cppMatrix, states);
        transitionWalker.walk(states, transitions);

        //statePrinter.print(states);
        TransitionPrinter transitionPrinter = new TransitionPrinter();
        transitionPrinter.print(transitions);
    }

}
