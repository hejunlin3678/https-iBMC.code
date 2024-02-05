import Date from 'src/app/common-app/utils/date';

export function displayDateTime(reportTimeStamp: string, reportTimeZone: string) {
    if (/^\d+$/.test(reportTimeStamp) || /^\d+$/.test(reportTimeZone)) {
        const date = parseInt(reportTimeStamp, 10);
        const timeZone = parseInt(reportTimeZone, 10) * 60;
        const time = new Date((date + timeZone) * 1000).format('yyyy-MM-dd hh:mm:ss');
        return String(time);
    } else {
        return '--';
    }
}
