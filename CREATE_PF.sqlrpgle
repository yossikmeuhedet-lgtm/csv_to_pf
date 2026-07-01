
        dcl-pi CREATE_PF extpgm;
          outfile_name char(10);
          in_quary char(2048);
          success ind;
        end-pi;

        dcl-s sql_string varchar(2200);

        // build sql
        sql_string = 'create table qtemp/' + %trim(outfile_name) +
                     ' (' + %trim(%xlate(x'00': ' ': in_quary)) + ' )';
        //prepare
        exec sql prepare out_table from :sql_string;
        //execute
        exec sql execute out_table;

        if sqlcode = 0;
          success = *on;
        else;
          success = *off;
        endif;

        *inlr=*on;
        return;
