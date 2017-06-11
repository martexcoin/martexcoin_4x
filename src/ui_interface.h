// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UI_INTERFACE_H
#define BITCOIN_UI_INTERFACE_H

#include <boost/signals2/last_value.hpp>
#include <boost/signals2/signal.hpp>

#include <string>

#include <stdint.h>

class CValverdeNodeConfig;
class CBasicKeyStore;
class CWallet;
class uint256;

/** General change type (added, updated, removed). */
enum ChangeType
{
    CT_NEW,
    CT_UPDATED,
    CT_DELETED
};

/** Signals for UI communication. */
class CClientUIInterface
{
public:
    /** Flags for CClientUIInterface::ThreadSafeMessageBox */
    enum MessageBoxFlags
    {
        YES                   = 0x00000002,
        OK                    = 0x00000004,
        NO                    = 0x00000008,
        YES_NO                = (YES|NO),
        CANCEL                = 0x00000010,
        APPLY                 = 0x00000020,
        CLOSE                 = 0x00000040,
        OK_DEFAULT            = 0x00000000,
        YES_DEFAULT           = 0x00000000,
        NO_DEFAULT            = 0x00000080,
        CANCEL_DEFAULT        = 0x80000000,
        ICON_EXCLAMATION      = 0x00000100,
        ICON_HAND             = 0x00000200,
        ICON_WARNING          = ICON_EXCLAMATION,
        ICON_ERROR            = ICON_HAND,
        ICON_QUESTION         = 0x00000400,
        ICON_INFORMATION      = 0x00000800,
        ICON_STOP             = ICON_HAND,
        ICON_ASTERISK         = ICON_INFORMATION,
        ICON_MASK             = (0x00000100|0x00000200|0x00000400|0x00000800),
        FORWARD               = 0x00001000,
        BACKWARD              = 0x00002000,
        RESET                 = 0x00004000,
        HELP                  = 0x00008000,
        MORE                  = 0x00010000,
        SETUP                 = 0x00020000,
        // Force blocking, modal message box dialog (not just OS notification)
        MODAL                 = 0x00040000,

	/** These values are taken from qmessagebox.h "enum StandardButton" to be directly usable */
	BTN_OK = 0x00000400U, // QMessageBox::Ok
	BTN_YES = 0x00004000U, // QMessageBox::Yes
	BTN_NO = 0x00010000U, // QMessageBox::No
	BTN_ABORT = 0x00040000U, // QMessageBox::Abort
	BTN_RETRY = 0x00080000U, // QMessageBox::Retry
	BTN_IGNORE = 0x00100000U, // QMessageBox::Ignore
	BTN_CLOSE = 0x00200000U, // QMessageBox::Close
	BTN_CANCEL = 0x00400000U, // QMessageBox::Cancel
	BTN_DISCARD = 0x00800000U, // QMessageBox::Discard
	BTN_HELP = 0x01000000U, // QMessageBox::Help
	BTN_APPLY = 0x02000000U, // QMessageBox::Apply
	BTN_RESET = 0x04000000U, // QMessageBox::Reset
	/**
	* Mask of all available buttons in CClientUIInterface::MessageBoxFlags
	* This needs to be updated, when buttons are changed there!
	*/
	BTN_MASK = (BTN_OK | BTN_YES | BTN_NO | BTN_ABORT | BTN_RETRY | BTN_IGNORE |
	BTN_CLOSE | BTN_CANCEL | BTN_DISCARD | BTN_HELP | BTN_APPLY | BTN_RESET),
	/** Predefined combinations for certain default usage cases */
	MSG_INFORMATION = (ICON_INFORMATION | BTN_OK),
	MSG_WARNING = (ICON_WARNING | BTN_OK | MODAL),
	MSG_ERROR = (ICON_ERROR | BTN_OK | MODAL)

    };

    /** Show message box. */
    boost::signals2::signal<void (const std::string& message, const std::string& caption, int style)> ThreadSafeMessageBox;

    /** Ask the user whether they want to pay a fee or not. */
    boost::signals2::signal<bool (int64_t nFeeRequired, const std::string& strCaption), boost::signals2::last_value<bool> > ThreadSafeAskFee;

    /** Handle a URL passed at the command line. */
    boost::signals2::signal<void (const std::string& strURI)> ThreadSafeHandleURI;

    /** Progress message during initialization. */
    boost::signals2::signal<void (const std::string &message)> InitMessage;

    /** Initiate client shutdown. */
    boost::signals2::signal<void ()> QueueShutdown;

    /** Translate a message to the native language of the user. */
    boost::signals2::signal<std::string (const char* psz)> Translate;

    /** Block chain changed. */
    boost::signals2::signal<void ()> NotifyBlocksChanged;

    /** Number of network connections changed. */
    boost::signals2::signal<void (int newNumConnections)> NotifyNumConnectionsChanged;

    /**
     * New, updated or cancelled alert.
     * @note called with lock cs_mapAlerts held.
     */
    boost::signals2::signal<void (const uint256 &hash, ChangeType status)> NotifyAlertChanged;

    boost::signals2::signal<void (CValverdeNodeConfig nodeConfig)> NotifyValverdeNodeChanged;
};

extern CClientUIInterface uiInterface;

/**
 * Translation function: Call Translate signal on UI interface, which returns a boost::optional result.
 * If no translation slot is registered, nothing is returned, and simply return the input.
 */
inline std::string _(const char* psz)
{
    boost::optional<std::string> rv = uiInterface.Translate(psz);
    return rv ? (*rv) : psz;
}

#endif
