function send_notification
{
if [ -s $MAIL_FILE -a  "$MAILOUT" = "TRUE" ];
then
        case $(uname) in
        AIX|HP-UX|Linux) SENDMAIL="/usr/sbin/sendmail"
                         ;;
        SunOS)           SENDMAIL="/usr/lib/sendmail"
                         ;;
        esac

        echo "\nSending e-mail notification"
        $SENDMAIL -f randy@$THISHOST $MAIL_LIST < $MAIL_FILE
fi
}

