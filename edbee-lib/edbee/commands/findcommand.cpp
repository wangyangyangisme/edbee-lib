/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "findcommand.h"

#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/models/textsearcher.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

/// Constructs the find operation
/// @param findType the findtype to use
FindCommand::FindCommand( FindType findType )
    : findType_( findType )
{
}


/// executes this command. What happens depends mainly on the FindType
/// @param controller the controller used for executing
void FindCommand::execute(TextEditorController* controller)
{
    TextSearcher* searcher = controller->textSearcher();
    TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controller->textSelection() );
    TextDocument* doc = controller->textDocument();

    // uses the current selection for find or uses the current word (doesn't require a soft undo)
    if( findType_ == UseSelectionForFind ) {
        // when empty select the word
        TextRange range = currentSelection->range(0); // Note: No reference we're changing TextRange copy!
        if( range.isEmpty() ) {
            range.expandToWord( doc, doc->config()->whitespaces(), doc->config()->charGroups());
        }
        searcher->setSearchTerm( doc->textPart( range.min(), range.length() ) );
        return;
    }

    // save the current selection

    TextRangeSet* sel = currentSelection; // temporary direct changing
//    TextRangeSet* sel = new TextRangeSet(*currentSelection);  // start with the current selection

    // next process the operation and move the caret/selection
    switch( findType_ ) {

        // finds the next match of the current selected word
        case FindNextMatch:
            searcher->setReverse(false);
            searcher->findNext( controller->widget() );
            break;

        // finds the previous match of the current selected word
        case FindPreviousMatch:
            searcher->setReverse(true);
            searcher->findNext( controller->widget() );
            break;

        // finds and selects the next match
        case SelectNextMatch:
            searcher->setReverse(false);
            searcher->selectNext( controller->widget() );
            break;

        // finds and selects the previous match
        case SelectPreviousMatch:
            searcher->setReverse(true);
            searcher->selectNext( controller->widget() );
            break;

        // selects all matches
        case SelectAllMatches:
            searcher->selectAll( controller->widget() );
            break;

        // use the current-selection for find or selects the current word
        case FindUnderExpand:
            // when no selection is used select it
            if( !sel->hasSelection() ) {
                TextRange& range = sel->range(0); // Note: No reference we're changing TextRange copy!
                if( range.isEmpty() ) {
                    range.expandToWord( doc, doc->config()->whitespaces(), doc->config()->charGroups());
                }
                searcher->setSearchTerm( doc->textPart( range.min(), range.length() ) );
            // next select the next word
            } else {
                searcher->setReverse(false);
                searcher->selectNext( controller->widget() );
            }
            break;

        default:
            qlog_warn() << "Warning, invalid FindCommand supplied!";
            Q_ASSERT(false);
    }


    // no change?
    /* Temporary disabled
    if( currentSelection->equals( *sel ) ) {
        delete sel;
        return; // 0
    }


    controller->changeAndGiveTextSelection( sel, 0 );
    */
    controller->update();

}


/// Converts this find command to a string
/// This string describes the command (useful only for debugging purposes)
QString FindCommand::toString()
{
    QString str;
    switch( findType_ ) {
        case UseSelectionForFind: str =  "UseSelectionForFind"; break;
        case FindNextMatch: str =  "FindNext"; break;
        case FindPreviousMatch: str = "FindPrevious"; break;
        case SelectNextMatch: str = "SelectNextMatch"; break;
        case SelectPreviousMatch: str = "SelectPreviousMatch"; break;
        case SelectAllMatches: str =  "SelectAllMatches"; break;
        case FindUnderExpand: str = "FindUnderExpand"; break;
    }
    return QString("FindCommand(%1)").arg(str);
}


} // edbee
