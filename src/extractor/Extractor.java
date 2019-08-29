package extractor;

import extractor.gears.StatePrinter;
import global.structure.Line;
import extractor.state.StateWalker;
import extractor.gears.Matrix;
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
    private StatePrinter instructionPrinter;

    //Constructor
    public Extractor () {
        matrix = new Matrix();
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
        instructionPrinter = StatePrinter.getInstance();
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

        //instructionPrinter.print(states);
    }

}
