package reader;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.TerminalNode;

//Class
import reader.CPP14Parser.ClassheadContext;
import reader.CPP14Parser.ClassnameContext;
import reader.CPP14Parser.ClasskeyContext;
import reader.CPP14Parser.ClassordecltypeContext;
import reader.CPP14Parser.ClassheadnameContext;
import reader.CPP14Parser.ClassspecifierContext;
import reader.CPP14Parser.ClassvirtspecifierContext;

//Attribute
import reader.CPP14Parser.AttributespecifierseqContext;
import reader.CPP14Parser.AttributelistContext;
import reader.CPP14Parser.AttributeargumentclauseContext;
import reader.CPP14Parser.AttributeContext;
import reader.CPP14Parser.AttributedeclarationContext;
import reader.CPP14Parser.AttributenamespaceContext;
import reader.CPP14Parser.AttributescopedtokenContext;
import reader.CPP14Parser.AttributespecifierContext;
import reader.CPP14Parser.AttributetokenContext;

//Expression
import reader.CPP14Parser.ExpressionContext;
import reader.CPP14Parser.ExpressionlistContext;

import reader.CPP14Parser.ConstantexpressionContext;
import reader.CPP14Parser.PostfixexpressionContext;
import reader.CPP14Parser.RelationalexpressionContext;
import reader.CPP14Parser.LogicalorexpressionContext;
import reader.CPP14Parser.MultiplicativeexpressionContext;
import reader.CPP14Parser.AdditiveexpressionContext;
import reader.CPP14Parser.EqualityexpressionContext;
import reader.CPP14Parser.IdexpressionContext;
import reader.CPP14Parser.PmexpressionContext;
import reader.CPP14Parser.ShiftexpressionContext;
import reader.CPP14Parser.AndexpressionContext;
import reader.CPP14Parser.ExclusiveorexpressionContext;
import reader.CPP14Parser.InclusiveorexpressionContext;
import reader.CPP14Parser.LogicalandexpressionContext;
import reader.CPP14Parser.AssignmentexpressionContext;
import reader.CPP14Parser.CastexpressionContext;
import reader.CPP14Parser.UnaryexpressionContext;
import reader.CPP14Parser.DeleteexpressionContext;
import reader.CPP14Parser.LambdaexpressionContext;
import reader.CPP14Parser.NewexpressionContext;
import reader.CPP14Parser.NoexceptexpressionContext;
import reader.CPP14Parser.PrimaryexpressionContext;
import reader.CPP14Parser.ThrowexpressionContext;

//Namespace
import reader.CPP14Parser.NamespacebodyContext;
import reader.CPP14Parser.NamespacenameContext;
import reader.CPP14Parser.NamespacealiasContext;
import reader.CPP14Parser.NamespacealiasdefinitionContext;
import reader.CPP14Parser.NamespacedefinitionContext;

import reader.CPP14Parser.OriginalnamespacenameContext;
import reader.CPP14Parser.OriginalnamespacedefinitionContext;
import reader.CPP14Parser.ExtensionnamespacedefinitionContext;
import reader.CPP14Parser.NamednamespacedefinitionContext;
import reader.CPP14Parser.QualifiednamespacespecifierContext;
import reader.CPP14Parser.UnnamednamespacedefinitionContext;

//Condition
import reader.CPP14Parser.ConditionalexpressionContext;
import reader.CPP14Parser.ConditionContext;

//Parameter
import reader.CPP14Parser.ParameterdeclarationlistContext;
import reader.CPP14Parser.ParameterdeclarationclauseContext;
import reader.CPP14Parser.ParameterdeclarationContext;
import reader.CPP14Parser.ParametersandqualifiersContext;

//Function
import reader.CPP14Parser.FunctiondefinitionContext;
import reader.CPP14Parser.FunctionbodyContext;
import reader.CPP14Parser.FunctionspecifierContext;

//Exception
import reader.CPP14Parser.ExceptiondeclarationContext;
import reader.CPP14Parser.ExceptionspecificationContext;
import reader.CPP14Parser.TryblockContext;
import reader.CPP14Parser.FunctiontryblockContext;

//Statement
import reader.CPP14Parser.StatementContext;
import reader.CPP14Parser.StatementseqContext;

import reader.CPP14Parser.IterationstatementContext;
import reader.CPP14Parser.JumpstatementContext;
import reader.CPP14Parser.LabeledstatementContext;
import reader.CPP14Parser.ForinitstatementContext;
import reader.CPP14Parser.SelectionstatementContext;
import reader.CPP14Parser.CompoundstatementContext;
import reader.CPP14Parser.ExpressionstatementContext;

//Type
import reader.CPP14Parser.TypespecifierseqContext;
import reader.CPP14Parser.TypeidlistContext;
import reader.CPP14Parser.TypenamespecifierContext;
import reader.CPP14Parser.TypespecifierContext;
import reader.CPP14Parser.TypedefnameContext;
import reader.CPP14Parser.TypeidofthetypeidContext;
import reader.CPP14Parser.TypeparameterContext;
import reader.CPP14Parser.TypeidofexprContext;

import reader.CPP14Parser.ThetypeidContext;
import reader.CPP14Parser.ThetypenameContext;
import reader.CPP14Parser.SimpletypespecifierContext;
import reader.CPP14Parser.TrailingtypespecifierContext;
import reader.CPP14Parser.TrailingtypespecifierseqContext;
import reader.CPP14Parser.NewtypeidContext;
import reader.CPP14Parser.TrailingreturntypeContext;
import reader.CPP14Parser.ConversiontypeidContext;
import reader.CPP14Parser.BasetypespecifierContext;
import reader.CPP14Parser.ElaboratedtypespecifierContext;
import reader.CPP14Parser.DecltypespecifierContext;

//Declaration
import reader.CPP14Parser.DeclaratorContext;
import reader.CPP14Parser.DeclspecifierseqContext;
import reader.CPP14Parser.DeclarationseqContext;
import reader.CPP14Parser.DeclarationContext;

import reader.CPP14Parser.NoptrabstractdeclaratorContext;
import reader.CPP14Parser.NoptrdeclaratorContext;
import reader.CPP14Parser.NoptrabstractpackdeclaratorContext;
import reader.CPP14Parser.InitdeclaratorlistContext;
import reader.CPP14Parser.MemberdeclaratorlistContext;
import reader.CPP14Parser.NoptrnewdeclaratorContext;
import reader.CPP14Parser.AbstractdeclaratorContext;
import reader.CPP14Parser.PtrabstractdeclaratorContext;
import reader.CPP14Parser.PtrdeclaratorContext;
import reader.CPP14Parser.AbstractpackdeclaratorContext;
import reader.CPP14Parser.AliasdeclarationContext;
import reader.CPP14Parser.BlockdeclarationContext;
import reader.CPP14Parser.ConversiondeclaratorContext;
import reader.CPP14Parser.EmptydeclarationContext;
import reader.CPP14Parser.NewdeclaratorContext;
import reader.CPP14Parser.SimpledeclarationContext;
import reader.CPP14Parser.Static_assertdeclarationContext;
import reader.CPP14Parser.TemplatedeclarationContext;
import reader.CPP14Parser.UsingdeclarationContext;
import reader.CPP14Parser.DeclarationstatementContext;
import reader.CPP14Parser.DeclaratoridContext;
import reader.CPP14Parser.DeclspecifierContext;
import reader.CPP14Parser.ForrangedeclarationContext;
import reader.CPP14Parser.InitdeclaratorContext;
import reader.CPP14Parser.LambdadeclaratorContext;
import reader.CPP14Parser.MemberdeclarationContext;
import reader.CPP14Parser.MemberdeclaratorContext;
import reader.CPP14Parser.OpaqueenumdeclarationContext;

//Others
import reader.CPP14Parser.NestednamespecifierContext;
import reader.CPP14Parser.BracedinitlistContext;
import reader.CPP14Parser.SimpletemplateidContext;
import reader.CPP14Parser.BalancedtokenseqContext;
import reader.CPP14Parser.InitializerlistContext;
import reader.CPP14Parser.TemplateargumentlistContext;
import reader.CPP14Parser.TemplateparameterlistContext;
import reader.CPP14Parser.VirtspecifierseqContext;
import reader.CPP14Parser.InitializerclauseContext;
import reader.CPP14Parser.PtroperatorContext;
import reader.CPP14Parser.BasespecifierlistContext;
import reader.CPP14Parser.CapturelistContext;
import reader.CPP14Parser.EnumeratorlistContext;
import reader.CPP14Parser.CvqualifierseqContext;
import reader.CPP14Parser.HandlerseqContext;
import reader.CPP14Parser.UnqualifiedidContext;
import reader.CPP14Parser.PurespecifierContext;
import reader.CPP14Parser.AccessspecifierContext;
import reader.CPP14Parser.BraceorequalinitializerContext;
import reader.CPP14Parser.CtorinitializerContext;
import reader.CPP14Parser.CvqualifierContext;
import reader.CPP14Parser.EnumbaseContext;
import reader.CPP14Parser.EnumkeyContext;
import reader.CPP14Parser.InitializerContext;
import reader.CPP14Parser.LiteraloperatoridContext;
import reader.CPP14Parser.MemberspecificationContext;
import reader.CPP14Parser.MeminitializerlistContext;
import reader.CPP14Parser.OperatorfunctionidContext;
import reader.CPP14Parser.RightShiftAssignContext;
import reader.CPP14Parser.RightShiftContext;
import reader.CPP14Parser.AlignmentspecifierContext;
import reader.CPP14Parser.AsmdefinitionContext;
import reader.CPP14Parser.AssignmentoperatorContext;
import reader.CPP14Parser.BalancedtokenContext;
import reader.CPP14Parser.BaseclauseContext;
import reader.CPP14Parser.BasespecifierContext;
import reader.CPP14Parser.BooleanliteralContext;
import reader.CPP14Parser.CaptureContext;
import reader.CPP14Parser.CapturedefaultContext;
import reader.CPP14Parser.ConversionfunctionidContext;
import reader.CPP14Parser.DynamicexceptionspecificationContext;
import reader.CPP14Parser.EnumeratorContext;
import reader.CPP14Parser.EnumeratordefinitionContext;
import reader.CPP14Parser.EnumheadContext;
import reader.CPP14Parser.EnumnameContext;
import reader.CPP14Parser.EnumspecifierContext;
import reader.CPP14Parser.ExplicitinstantiationContext;
import reader.CPP14Parser.ExplicitspecializationContext;
import reader.CPP14Parser.ForrangeinitializerContext;
import reader.CPP14Parser.HandlerContext;
import reader.CPP14Parser.InitcaptureContext;
import reader.CPP14Parser.LambdacaptureContext;
import reader.CPP14Parser.LambdaintroducerContext;
import reader.CPP14Parser.LinkagespecificationContext;
import reader.CPP14Parser.LiteralContext;
import reader.CPP14Parser.MeminitializerContext;
import reader.CPP14Parser.MeminitializeridContext;
import reader.CPP14Parser.NewinitializerContext;
import reader.CPP14Parser.NewplacementContext;
import reader.CPP14Parser.NoexceptspecificationContext;
import reader.CPP14Parser.PointerliteralContext;
import reader.CPP14Parser.PseudodestructornameContext;
import reader.CPP14Parser.QualifiedidContext;
import reader.CPP14Parser.RefqualifierContext;
import reader.CPP14Parser.SimplecaptureContext;
import reader.CPP14Parser.StorageclassspecifierContext;
import reader.CPP14Parser.TemplateargumentContext;
import reader.CPP14Parser.TemplateidContext;
import reader.CPP14Parser.TemplatenameContext;
import reader.CPP14Parser.TemplateparameterContext;
import reader.CPP14Parser.TheoperatorContext;
import reader.CPP14Parser.UnaryoperatorContext;
import reader.CPP14Parser.UserdefinedliteralContext;
import reader.CPP14Parser.UsingdirectiveContext;
import reader.CPP14Parser.VirtspecifierContext;
import reader.CPP14Parser.TranslationunitContext;

public class ParseTreePrinter {
    //Attributes
    private int level;
    private int classLevel;
    private boolean visibility;
    private boolean forStructure;
    private boolean newLine;

    //Constructor
    public ParseTreePrinter() {
        level = 0;
        classLevel = -1;
        visibility = false;
        forStructure = false;
        newLine = false;
    }

    //Classified Methods
    public void printAttributeData (ParseTree parseTree) {
        if (parseTree instanceof AttributespecifierseqContext) {
            System.out.println("AttributespecifierseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributelistContext) {
            System.out.println("AttributelistContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributeargumentclauseContext) {
            System.out.println("AttributeargumentclauseContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributeContext) {
            System.out.println("AttributeContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributedeclarationContext) {
            System.out.println("AttributedeclarationContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributenamespaceContext) {
            System.out.println("AttributenamespaceContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributescopedtokenContext) {
            System.out.println("AttributescopedtokenContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributespecifierContext) {
            System.out.println("AttributespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof AttributetokenContext) {
            System.out.println("AttributetokenContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printAttributeData(parseTree.getChild(i));
        }
    }

    public void printClassData (ParseTree parseTree) {
        if (parseTree instanceof ClassheadContext) {
            System.out.println("ClassheadContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClassnameContext) {
            System.out.println("ClassnameContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClasskeyContext) {
            System.out.println("ClasskeyContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClassordecltypeContext) {
            System.out.println("ClassordecltypeContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClassheadnameContext) {
            System.out.println("ClassheadnameContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClassspecifierContext) {
            System.out.println("ClassspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof ClassvirtspecifierContext) {
            System.out.println("ClassvirtspecifierContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printClassData(parseTree.getChild(i));
        }
    }

    public void printExpressionData (ParseTree parseTree, boolean complete) {
        if (parseTree instanceof ExpressionContext) {
            System.out.println("ExpressionContext: " + parseTree.getText());
        }
        if (parseTree instanceof ExpressionlistContext) {
            System.out.println("ExpressionlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof ExpressionstatementContext) {
            System.out.println("ExpressionstatementContext: " + parseTree.getText());
        }
        if (complete) {
            if (parseTree instanceof ConstantexpressionContext) {
                System.out.println("ConstantexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof PostfixexpressionContext) {
                System.out.println("PostfixexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof RelationalexpressionContext) {
                System.out.println("RelationalexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof LogicalorexpressionContext) {
                System.out.println("LogicalorexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof MultiplicativeexpressionContext) {
                System.out.println("MultiplicativeexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof AdditiveexpressionContext) {
                System.out.println("AdditiveexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof EqualityexpressionContext) {
                System.out.println("EqualityexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof IdexpressionContext) {
                System.out.println("IdexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof PmexpressionContext) {
                System.out.println("PmexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof ShiftexpressionContext) {
                System.out.println("ShiftexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof AndexpressionContext) {
                System.out.println("AndexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof ExclusiveorexpressionContext) {
                System.out.println("ExclusiveorexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof InclusiveorexpressionContext) {
                System.out.println("InclusiveorexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof LogicalandexpressionContext) {
                System.out.println("LogicalandexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof AssignmentexpressionContext) {
                System.out.println("AssignmentexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof CastexpressionContext) {
                System.out.println("CastexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof ConditionalexpressionContext) {
                System.out.println("ConditionalexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof UnaryexpressionContext) {
                System.out.println("UnaryexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof DeleteexpressionContext) {
                System.out.println("DeleteexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof LambdaexpressionContext) {
                System.out.println("LambdaexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof NewexpressionContext) {
                System.out.println("NewexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof NoexceptexpressionContext) {
                System.out.println("NoexceptexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof PrimaryexpressionContext) {
                System.out.println("PrimaryexpressionContext: " + parseTree.getText());
            }
            if (parseTree instanceof ThrowexpressionContext) {
                System.out.println("ThrowexpressionContext: " + parseTree.getText());
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printExpressionData(parseTree.getChild(i), complete);
        }
    }

    public void printNameSpaceData (ParseTree parseTree, boolean complete) {
        if (parseTree instanceof NamespacebodyContext) {
            System.out.println("NamespacebodyContext: " + parseTree.getText());
        }
        if (parseTree instanceof NamespacenameContext) {
            System.out.println("NamespacenameContext: " + parseTree.getText());
        }
        if (parseTree instanceof NamespacealiasContext) {
            System.out.println("NamespacealiasContext: " + parseTree.getText());
        }
        if (parseTree instanceof NamespacealiasdefinitionContext) {
            System.out.println("NamespacealiasdefinitionContext: " + parseTree.getText());
        }
        if (parseTree instanceof NamespacedefinitionContext) {
            System.out.println("NamespacedefinitionContext: " + parseTree.getText());
        }
        if (complete) {
            if (parseTree instanceof OriginalnamespacenameContext) {
                System.out.println("OriginalnamespacenameContext: " + parseTree.getText());
            }
            if (parseTree instanceof OriginalnamespacedefinitionContext) {
                System.out.println("OriginalnamespacedefinitionContext: " + parseTree.getText());
            }
            if (parseTree instanceof ExtensionnamespacedefinitionContext) {
                System.out.println("ExtensionnamespacedefinitionContext: " + parseTree.getText());
            }
            if (parseTree instanceof NamednamespacedefinitionContext) {
                System.out.println("NamednamespacedefinitionContext: " + parseTree.getText());
            }
            if (parseTree instanceof QualifiednamespacespecifierContext) {
                System.out.println("QualifiednamespacespecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof UnnamednamespacedefinitionContext) {
                System.out.println("UnnamednamespacedefinitionContext: " + parseTree.getText());
            }
            if (parseTree instanceof AttributenamespaceContext) {
                System.out.println("AttributenamespaceContext: " + parseTree.getText());
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printNameSpaceData(parseTree.getChild(i), complete);
        }
    }

    public void printStatementData (ParseTree parseTree, boolean complete) {
        if (parseTree instanceof StatementContext) {
            System.out.println("StatementContext: " + parseTree.getText());
        }
        if (parseTree instanceof StatementseqContext) {
            System.out.println("StatementseqContext: " + parseTree.getText());
        }
        if (complete) {
            if (parseTree instanceof IterationstatementContext) {
                System.out.println("IterationstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof JumpstatementContext) {
                System.out.println("JumpstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof LabeledstatementContext) {
                System.out.println("LabeledstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof ForinitstatementContext) {
                System.out.println("ForinitstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof SelectionstatementContext) {
                System.out.println("SelectionstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof CompoundstatementContext) {
                System.out.println("CompoundstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof ExpressionstatementContext) {
                System.out.println("ExpressionstatementContext: " + parseTree.getText());
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printStatementData(parseTree.getChild(i), complete);
        }
    }

    public void printTypeData (ParseTree parseTree, boolean complete) {
        if (parseTree instanceof TypespecifierseqContext) {
            System.out.println("TypespecifierseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypeidlistContext) {
            System.out.println("TypeidlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypenamespecifierContext) {
            System.out.println("TypenamespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypespecifierContext) {
            System.out.println("TypespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypedefnameContext) {
            System.out.println("TypedefnameContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypeidofthetypeidContext) {
            System.out.println("TypeidofthetypeidContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypeparameterContext) {
            System.out.println("TypeparameterContext: " + parseTree.getText());
        }
        if (parseTree instanceof TypeidofexprContext) {
            System.out.println("TypeidofexprContext: " + parseTree.getText());
        }
        if (complete) {
            if (parseTree instanceof ThetypeidContext) {
                System.out.println("ThetypeidContext: " + parseTree.getText());
            }
            if (parseTree instanceof ThetypenameContext) {
                System.out.println("ThetypenameContext: " + parseTree.getText());
            }
            if (parseTree instanceof SimpletypespecifierContext) {
                System.out.println("SimpletypespecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof TrailingtypespecifierContext) {
                System.out.println("TrailingtypespecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof TrailingtypespecifierseqContext) {
                System.out.println("TrailingtypespecifierseqContext: " + parseTree.getText());
            }
            if (parseTree instanceof NewtypeidContext) {
                System.out.println("NewtypeidContext: " + parseTree.getText());
            }
            if (parseTree instanceof TrailingreturntypeContext) {
                System.out.println("TrailingreturntypeContext: " + parseTree.getText());
            }
            if (parseTree instanceof ConversiontypeidContext) {
                System.out.println("ConversiontypeidContext: " + parseTree.getText());
            }
            if (parseTree instanceof BasetypespecifierContext) {
                System.out.println("BasetypespecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof ElaboratedtypespecifierContext) {
                System.out.println("ElaboratedtypespecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof DecltypespecifierContext) {
                System.out.println("DecltypespecifierContext: " + parseTree.getText());
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printTypeData(parseTree.getChild(i), complete);
        }
    }

    public void printDeclarationData (ParseTree parseTree, boolean complete) {
        if (parseTree instanceof DeclaratorContext) {
            System.out.println("DeclaratorContext: " + parseTree.getText());
        }
        if (parseTree instanceof DeclspecifierseqContext) {
            System.out.println("DeclspecifierseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof DeclarationseqContext) {
            System.out.println("DeclarationseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof DeclarationContext) {
            System.out.println("DeclarationContext: " + parseTree.getText());
        }
        if (complete) {
            if (parseTree instanceof NoptrabstractdeclaratorContext) {
                System.out.println("NoptrabstractdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof NoptrdeclaratorContext) {
                System.out.println("NoptrdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof NoptrabstractpackdeclaratorContext) {
                System.out.println("NoptrabstractpackdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof InitdeclaratorlistContext) {
                System.out.println("InitdeclaratorlistContext: " + parseTree.getText());
            }
            if (parseTree instanceof MemberdeclaratorlistContext) {
                System.out.println("MemberdeclaratorlistContext: " + parseTree.getText());
            }
            if (parseTree instanceof NoptrnewdeclaratorContext) {
                System.out.println("NoptrnewdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof AbstractdeclaratorContext) {
                System.out.println("AbstractdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof PtrabstractdeclaratorContext) {
                System.out.println("PtrabstractdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof PtrdeclaratorContext) {
                System.out.println("PtrdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof AbstractpackdeclaratorContext) {
                System.out.println("AbstractpackdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof AliasdeclarationContext) {
                System.out.println("AliasdeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof BlockdeclarationContext) {
                System.out.println("BlockdeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof ConversiondeclaratorContext) {
                System.out.println("ConversiondeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof EmptydeclarationContext) {
                System.out.println("EmptydeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof NewdeclaratorContext) {
                System.out.println("NewdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof SimpledeclarationContext) {
                System.out.println("SimpledeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof Static_assertdeclarationContext) {
                System.out.println("Static_assertdeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof TemplatedeclarationContext) {
                System.out.println("TemplatedeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof UsingdeclarationContext) {
                System.out.println("UsingdeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof DeclarationstatementContext) {
                System.out.println("DeclarationstatementContext: " + parseTree.getText());
            }
            if (parseTree instanceof DeclaratoridContext) {
                System.out.println("DeclaratoridContext: " + parseTree.getText());
            }
            if (parseTree instanceof DeclspecifierContext) {
                System.out.println("DeclspecifierContext: " + parseTree.getText());
            }
            if (parseTree instanceof ForrangedeclarationContext) {
                System.out.println("ForrangedeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof InitdeclaratorContext) {
                System.out.println("InitdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof LambdadeclaratorContext) {
                System.out.println("LambdadeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof MemberdeclarationContext) {
                System.out.println("MemberdeclarationContext: " + parseTree.getText());
            }
            if (parseTree instanceof MemberdeclaratorContext) {
                System.out.println("MemberdeclaratorContext: " + parseTree.getText());
            }
            if (parseTree instanceof OpaqueenumdeclarationContext) {
                System.out.println("OpaqueenumdeclarationContext: " + parseTree.getText());
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printDeclarationData(parseTree.getChild(i), complete);
        }
    }

    public void printConditionData (ParseTree parseTree) {
        if (parseTree instanceof ConditionalexpressionContext) {
            System.out.println("ConditionalexpressionContext: " + parseTree.getText());
        }
        if (parseTree instanceof ConditionContext) {
            System.out.println("ConditionContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printConditionData(parseTree.getChild(i));
        }
    }

    public void printParameterData (ParseTree parseTree) {
        if (parseTree instanceof ParameterdeclarationlistContext) {
            System.out.println("ParameterdeclarationlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof ParameterdeclarationclauseContext) {
            System.out.println("ParameterdeclarationclauseContext: " + parseTree.getText());
        }
        if (parseTree instanceof ParameterdeclarationContext) {
            System.out.println("ParameterdeclarationContext: " + parseTree.getText());
        }
        if (parseTree instanceof ParametersandqualifiersContext) {
            System.out.println("ParametersandqualifiersContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printParameterData(parseTree.getChild(i));
        }
    }

    public void printFunctionData (ParseTree parseTree) {
        if (parseTree instanceof FunctiondefinitionContext) {
            System.out.println("FunctiondefinitionContext: " + parseTree.getText());
        }
        if (parseTree instanceof FunctionbodyContext) {
            System.out.println("FunctionbodyContext: " + parseTree.getText());
        }
        if (parseTree instanceof FunctionspecifierContext) {
            System.out.println("FunctionspecifierContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printFunctionData(parseTree.getChild(i));
        }
    }

    public void printExceptionData (ParseTree parseTree) {
        if (parseTree instanceof ExceptiondeclarationContext) {
            System.out.println("ExceptiondeclarationContext: " + parseTree.getText());
        }
        if (parseTree instanceof ExceptionspecificationContext) {
            System.out.println("ExceptionspecificationContext: " + parseTree.getText());
        }
        if (parseTree instanceof TryblockContext) {
            System.out.println("TryblockContext: " + parseTree.getText());
        }
        if (parseTree instanceof FunctiontryblockContext) {
            System.out.println("FunctiontryblockContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printExceptionData(parseTree.getChild(i));
        }
    }

    public void printOthersData (ParseTree parseTree) {
        if (parseTree instanceof NestednamespecifierContext) {
            System.out.println("NestednamespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof BracedinitlistContext) {
            System.out.println("BracedinitlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof SimpletemplateidContext) {
            System.out.println("SimpletemplateidContext: " + parseTree.getText());
        }
        if (parseTree instanceof BalancedtokenseqContext) {
            System.out.println("BalancedtokenseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof InitializerlistContext) {
            System.out.println("InitializerlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplateargumentlistContext) {
            System.out.println("TemplateargumentlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplateparameterlistContext) {
            System.out.println("TemplateparameterlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof VirtspecifierseqContext) {
            System.out.println("VirtspecifierseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof InitializerclauseContext) {
            System.out.println("InitializerclauseContext: " + parseTree.getText());
        }
        if (parseTree instanceof PtroperatorContext) {
            System.out.println("PtroperatorContext: " + parseTree.getText());
        }
        if (parseTree instanceof BasespecifierlistContext) {
            System.out.println("BasespecifierlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof CapturelistContext) {
            System.out.println("CapturelistContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumeratorlistContext) {
            System.out.println("EnumeratorlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof CvqualifierseqContext) {
            System.out.println("CvqualifierseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof HandlerseqContext) {
            System.out.println("HandlerseqContext: " + parseTree.getText());
        }
        if (parseTree instanceof UnqualifiedidContext) {
            System.out.println("UnqualifiedidContext: " + parseTree.getText());
        }
        if (parseTree instanceof PurespecifierContext) {
            System.out.println("PurespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof AccessspecifierContext) {
            System.out.println("AccessspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof BraceorequalinitializerContext) {
            System.out.println("BraceorequalinitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof CtorinitializerContext) {
            System.out.println("CtorinitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof CvqualifierContext) {
            System.out.println("CvqualifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumbaseContext) {
            System.out.println("EnumbaseContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumkeyContext) {
            System.out.println("EnumkeyContext: " + parseTree.getText());
        }
        if (parseTree instanceof InitializerContext) {
            System.out.println("InitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof LiteraloperatoridContext) {
            System.out.println("LiteraloperatoridContext: " + parseTree.getText());
        }
        if (parseTree instanceof MemberspecificationContext) {
            System.out.println("MemberspecificationContext: " + parseTree.getText());
        }
        if (parseTree instanceof MeminitializerlistContext) {
            System.out.println("MeminitializerlistContext: " + parseTree.getText());
        }
        if (parseTree instanceof OperatorfunctionidContext) {
            System.out.println("OperatorfunctionidContext: " + parseTree.getText());
        }
        if (parseTree instanceof RightShiftAssignContext) {
            System.out.println("RightShiftAssignContext: " + parseTree.getText());
        }
        if (parseTree instanceof RightShiftContext) {
            System.out.println("RightShiftContext: " + parseTree.getText());
        }
        if (parseTree instanceof AlignmentspecifierContext) {
            System.out.println("AlignmentspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof AsmdefinitionContext) {
            System.out.println("AsmdefinitionContext: " + parseTree.getText());
        }
        if (parseTree instanceof AssignmentoperatorContext) {
            System.out.println("AssignmentoperatorContext: " + parseTree.getText());
        }
        if (parseTree instanceof BalancedtokenContext) {
            System.out.println("BalancedtokenContext: " + parseTree.getText());
        }
        if (parseTree instanceof BaseclauseContext) {
            System.out.println("BaseclauseContext: " + parseTree.getText());
        }
        if (parseTree instanceof BasespecifierContext) {
            System.out.println("BasespecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof BooleanliteralContext) {
            System.out.println("BooleanliteralContext: " + parseTree.getText());
        }
        if (parseTree instanceof CaptureContext) {
            System.out.println("CaptureContext: " + parseTree.getText());
        }
        if (parseTree instanceof CapturedefaultContext) {
            System.out.println("CapturedefaultContext: " + parseTree.getText());
        }
        if (parseTree instanceof ConversionfunctionidContext) {
            System.out.println("ConversionfunctionidContext: " + parseTree.getText());
        }
        if (parseTree instanceof DynamicexceptionspecificationContext) {
            System.out.println("DynamicexceptionspecificationContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumeratorContext) {
            System.out.println("EnumeratorContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumeratordefinitionContext) {
            System.out.println("EnumeratordefinitionContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumheadContext) {
            System.out.println("EnumheadContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumnameContext) {
            System.out.println("EnumnameContext: " + parseTree.getText());
        }
        if (parseTree instanceof EnumspecifierContext) {
            System.out.println("EnumspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof ExplicitinstantiationContext) {
            System.out.println("ExplicitinstantiationContext: " + parseTree.getText());
        }
        if (parseTree instanceof ExplicitspecializationContext) {
            System.out.println("ExplicitspecializationContext: " + parseTree.getText());
        }
        if (parseTree instanceof ForrangeinitializerContext) {
            System.out.println("ForrangeinitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof HandlerContext) {
            System.out.println("HandlerContext: " + parseTree.getText());
        }
        if (parseTree instanceof InitcaptureContext) {
            System.out.println("InitcaptureContext: " + parseTree.getText());
        }
        if (parseTree instanceof LambdacaptureContext) {
            System.out.println("LambdacaptureContext: " + parseTree.getText());
        }
        if (parseTree instanceof LambdaintroducerContext) {
            System.out.println("LambdaintroducerContext: " + parseTree.getText());
        }
        if (parseTree instanceof LinkagespecificationContext) {
            System.out.println("LinkagespecificationContext: " + parseTree.getText());
        }
        if (parseTree instanceof LiteralContext) {
            System.out.println("LiteralContext: " + parseTree.getText());
        }
        if (parseTree instanceof MeminitializerContext) {
            System.out.println("MeminitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof MeminitializeridContext) {
            System.out.println("MeminitializeridContext: " + parseTree.getText());
        }
        if (parseTree instanceof NewinitializerContext) {
            System.out.println("NewinitializerContext: " + parseTree.getText());
        }
        if (parseTree instanceof NewplacementContext) {
            System.out.println("NewplacementContext: " + parseTree.getText());
        }
        if (parseTree instanceof NoexceptspecificationContext) {
            System.out.println("NoexceptspecificationContext: " + parseTree.getText());
        }
        if (parseTree instanceof PointerliteralContext) {
            System.out.println("PointerliteralContext: " + parseTree.getText());
        }
        if (parseTree instanceof PseudodestructornameContext) {
            System.out.println("PseudodestructornameContext: " + parseTree.getText());
        }
        if (parseTree instanceof QualifiedidContext) {
            System.out.println("QualifiedidContext: " + parseTree.getText());
        }
        if (parseTree instanceof RefqualifierContext) {
            System.out.println("RefqualifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof SimplecaptureContext) {
            System.out.println("SimplecaptureContext: " + parseTree.getText());
        }
        if (parseTree instanceof StorageclassspecifierContext) {
            System.out.println("StorageclassspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplateargumentContext) {
            System.out.println("TemplateargumentContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplateidContext) {
            System.out.println("TemplateidContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplatenameContext) {
            System.out.println("TemplatenameContext: " + parseTree.getText());
        }
        if (parseTree instanceof TemplateparameterContext) {
            System.out.println("TemplateparameterContext: " + parseTree.getText());
        }
        if (parseTree instanceof TheoperatorContext) {
            System.out.println("TheoperatorContext: " + parseTree.getText());
        }
        if (parseTree instanceof UnaryoperatorContext) {
            System.out.println("UnaryoperatorContext: " + parseTree.getText());
        }
        if (parseTree instanceof UserdefinedliteralContext) {
            System.out.println("UserdefinedliteralContext: " + parseTree.getText());
        }
        if (parseTree instanceof UsingdirectiveContext) {
            System.out.println("UsingdirectiveContext: " + parseTree.getText());
        }
        if (parseTree instanceof VirtspecifierContext) {
            System.out.println("VirtspecifierContext: " + parseTree.getText());
        }
        if (parseTree instanceof TranslationunitContext) {
            System.out.println("TranslationunitContext: " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printOthersData(parseTree.getChild(i));
        }
    }

    //Generic Methods
    public void printTerminalNodes (ParseTree parseTree) {
        if (parseTree instanceof TerminalNode) {
            System.out.println(parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printTerminalNodes(parseTree.getChild(i));
        }
    }

    public void printIndentedCode (ParseTree parseTree) {
        if (parseTree instanceof TerminalNode) {
            switch (parseTree.getText()) {
                case "for":
                    forStructure = true;
                    newLine("for ");
                    break;
                case "class":
                    classLevel = level;
                    newLine("class ");
                    break;
                case "public":
                    visibility = true;
                    newLine("public");
                    break;
                case "private":
                    visibility = true;
                    newLine("private");
                    break;
                case "protected":
                    visibility = true;
                    newLine("protected");
                    break;
                case ":":
                    if (visibility) {
                        visibility = false;
                        System.out.print(":\n");
                        indent();
                    } else {
                        System.out.print(":");
                    }
                    break;
                case "{":
                    System.out.print("\n");
                    indent();
                    System.out.print("{\n");
                    level ++;
                    indent();
                    break;
                case "}":
                    System.out.print("\n");
                    forStructure = false;
                    level --;
                    indent();
                    if (classLevel == level) {
                        System.out.print("}");
                        classLevel = -1;
                    } else {
                        System.out.print("}\n");
                        indent();
                    }
                    break;
                case ";":
                    if (forStructure) {
                        System.out.print("; ");
                    } else {
                        System.out.print(";");
                        newLine = true;
                    }
                    break;
                default:
                    newLine(parseTree.getText());
            }

        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printIndentedCode(parseTree.getChild(i));
        }
    }

    public void printEverything (ParseTree parseTree) {
        for (int i = 0; i < level; i++) {
            System.out.print("    ");
        }
        System.out.println(parseTree.getClass() + " -> " + parseTree.getText());
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            if (i == 0) {
                level ++;
            }
            printEverything(parseTree.getChild(i));
        }
        level --;
    }

    //Private Methods
    private void indent () {
        for (int i = 0; i < level; i++) {
            System.out.print("    ");
        }
    }

    private void newLine (String keyword) {
        if (newLine) {
            newLine = false;
            System.out.print("\n");
            indent();
            System.out.print(keyword + " ");
        } else {
            System.out.print(keyword + " ");
        }
    }
}
