package extractor;

import extractor.tools.InstructionPrinter;
import global.structure.Line;
import extractor.state.StateWalker;
import extractor.tools.Matrix;
import extractor.transition.TransitionWalker;
import global.structure.Instruction;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Extractor {
    //Attributes
    private Matrix matrix;
    private StateWalker stateWalker;
    private TransitionWalker transitionWalker;

    //Constructor
    public Extractor () {
        matrix = new Matrix();
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
    }

    //Public Methods
    public void extract (ParseTree parseTree) {
        List<Instruction> states = new ArrayList<>();
        List<Transition> transitions = new ArrayList<>();
        List<Line> cppMatrix = new ArrayList<>();
        cppMatrix.add(new Line());

        matrix.matrixGenerator(cppMatrix, parseTree);
        stateWalker.walk(cppMatrix, states);
        transitionWalker.walk(states, transitions);

        InstructionPrinter instructionPrinter = new InstructionPrinter();
        instructionPrinter.print(states);
    }

}
