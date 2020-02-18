%%
%% This is file `rian_article.cls',
%%
%% Adapted from Package `jss' to use with LaTeX2e for JSS publications (http://www.jstatsoft.org/) by Rian Pinheiro
%% rian.gabriel@gmail.com
%%
%% Package jss 
%% License: GPL-2
%% Copyright: (C) Achim Zeileis
%% Please report errors to Achim.Zeileis@R-project.org
%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{rian_article}[2015/07/30 v1.0 Modelo de Artigos]

\RequirePackage{ifthen}
%% options
\newif\if@article
\newif\if@codesnippet
\newif\if@bookreview
\newif\if@softwarereview
\newif\if@review
\newif\if@shortnames
\newif\if@nojss
\newif\if@notitle
\newif\if@noheadings
\newif\if@nofooter
% \newif\if@final

\@articletrue
\@codesnippetfalse
\@bookreviewfalse
\@softwarereviewfalse
\@reviewfalse
\@shortnamesfalse
\@nojssfalse
\@notitlefalse
\@noheadingsfalse
\@nofooterfalse


\DeclareOption{article}{\@articletrue%
  \@codesnippetfalse \@bookreviewfalse \@softwarereviewfalse}
\DeclareOption{codesnippet}{\@articlefalse%
  \@codesnippettrue \@bookreviewfalse \@softwarereviewfalse}
\DeclareOption{bookreview}{\@articlefalse%
  \@codesnippetfalse \@bookreviewtrue \@softwarereviewfalse}
\DeclareOption{softwarereview}{\@articlefalse%
  \@codesnippetfalse \@bookreviewfalse \@softwarereviewtrue}
\DeclareOption{shortnames}{\@shortnamestrue}
\DeclareOption{nojss}{\@nojsstrue}
\DeclareOption{notitle}{\@notitletrue}
\DeclareOption{noheadings}{\@noheadingstrue}
\DeclareOption{nofooter}{\@nofootertrue}


\ProcessOptions
\LoadClass[11pt,a4paper,twoside]{article}
%% required packages
\RequirePackage{graphicx,xcolor,ae,indentfirst}
\RequirePackage{fancyhdr}
\RequirePackage{fancyvrb}
\RequirePackage[T1]{fontenc}

\RequirePackage[utf8]{inputenc}
\RequirePackage[brazilian]{babel}

%% bibliography
\RequirePackage{natbib}
\bibliographystyle{agsm_url}

%% page layout
\topmargin 0pt
\textheight 46\baselineskip
\advance\textheight by \topskip
\oddsidemargin 0.125in
\evensidemargin 0.125in
\marginparwidth 0.75in
\textwidth 6.125in

\newcommand{\iclogo}{\vspace{1mm}\includegraphics[height=10mm,keepaspectratio]{ic}}

%% paragraphs
\setlength{\parskip}{0.7ex plus0.1ex minus0.1ex}
% \setlength{\parindent}{0em}
%% for all publications
% \newcommand{\Address}[1]{\def\@Address{#1}}
\newcommand{\Plaintitle}[1]{\def\@Plaintitle{#1}}
\newcommand{\Shorttitle}[1]{\def\@Shorttitle{#1}}
\newcommand{\Plainauthor}[1]{\def\@Plainauthor{#1}}
\newcommand{\Semestre}[1]{\def\@Semestre{#1}}
\newcommand{\Year}[1]{\def\@Year{#1}}
\newcommand{\Month}[1]{\def\@Month{#1}}
\newcommand{\Issue}[1]{\def\@Issue{#1}}
\newcommand{\Submitdate}[1]{\def\@Submitdate{#1}}
\newcommand{\Disciplina}[1]{\def\@Disciplina{#1}}
\newcommand{\Versao}[1]{\def\@Versao{#1}}
%% for articles and code snippets
\newcommand{\Acceptdate}[1]{\def\@Acceptdate{#1}}
\newcommand{\Abstract}[1]{\def\@Abstract{#1}}
\newcommand{\Keywords}[1]{\def\@Keywords{#1}}
\newcommand{\Plainkeywords}[1]{\def\@Plainkeywords{#1}}
\newcommand{\URL}[1]{\def\@URL{#1}}
%% for internal use
\newcommand{\Hypersubject}[1]{\def\@Hypersubject{#1}}
\newcommand{\Hyperauthor}[1]{\def\@Hyperauthor{#1}}
\newcommand{\Footername}[1]{\def\@Footername{#1}}
\newcommand{\Firstdate}[1]{\def\@Firstdate{#1}}
\newcommand{\Seconddate}[1]{\def\@Seconddate{#1}}
\newcommand{\Reviewauthor}[1]{\def\@Reviewauthor{#1}}
%% defaults
\author{Firstname Lastname\\Affiliation}
\title{Title}
\Abstract{---!!!---an abstract is required---!!!---}
\Plainauthor{\@author}
\Semestre{S}
\Year{YYYY}
\Month{MMMMMM}
\Issue{II}
\Submitdate{yyyy-mm-dd}
\Acceptdate{yyyy-mm-dd}
\Versao{0}

\Hypersubject{Trabalho final para disciplina \@Disciplina}
\Plaintitle{\@title}
\Shorttitle{\@title}
\Plainkeywords{\@Keywords}

\Hyperauthor{\@Plainauthor}
\Keywords{---!!!---at least one keyword is required---!!!---}
\Footername{}
\Firstdate{\textit{Entregue:} \@Submitdate}
\Seconddate{\textit{Corrigido:} \@Acceptdate}



%% footer
\newlength{\footerskip}
\setlength{\footerskip}{2.5\baselineskip plus 2ex minus 0.5ex}

\newcommand{\makefooter}{%
  \vspace{\footerskip}

  \if@nojss
    \begin{samepage}
    \textbf{\large \@Footername: \nopagebreak}\\[.3\baselineskip] \nopagebreak
    \end{samepage}
  \else
    \begin{samepage}

    \vfill
    \hrule \nopagebreak
    \vspace{.1\baselineskip}
    {\noindent\hspace{-.25cm}\fontfamily{pzc} \fontsize{13}{15} \selectfont \@Disciplina}
    \hfill
    Histórico do Artigo: \\ \nopagebreak
    Instituto de Computa\c c\~ao
    \hfill
    Versão \@Versao\\ \nopagebreak
    {Universidade Federal de Alagoas}
    \hfill
    \@Firstdate\\ \nopagebreak
%     \vspace{.3\baselineskip}
    \hrule
    \fancyfoot[LE]{}
    \fancyfoot[RO]{}
    \end{samepage}
  \fi
}
\if@nofooter
  %% \AtEndDocument{\makefooter}
\else
  \AtEndDocument{\makefooter}
\fi
%% required packages
\RequirePackage[unicode]{hyperref}
%% new \maketitle
\def\@myoddhead{
  {\color{white} JSS}\\[-1.42cm]
  \hspace{-2em} \includegraphics[height=23mm,keepaspectratio]{ufal} \hfill
  \parbox[b][23mm]{130mm}{\hrule height 3pt
   \center{
   {\fontfamily{pzc} \fontsize{28}{32} \selectfont \@Disciplina}
   \vfill
   {\it  {\fontfamily{pzc} \selectfont  Prof. Rian Gabriel S. Pinheiro, \@Year.\@Semestre}.%
            \hfill \href{http://professor.ufal.br/rian/}{http://professor.ufal.br/rian/}}}\\[0.1cm]
     \hrule height 3pt}}

     
%%%%%%%%%%%%%% MAKETITLE  %%%%%%%%%%%%%%%%%
\def\maketitle{
  \@oddhead{\@myoddhead} \par

  \begingroup
    \def\thefootnote{\fnsymbol{footnote}}
    \def\@makefnmark{\hbox to 0pt{$^{\@thefnmark}$\hss}}
    \long\def\@makefntext##1{\parindent 1em\noindent
			    \hbox to1.8em{\hss $\m@th ^{\@thefnmark}$}##1}
    \@maketitle \@thanks
  \endgroup
  \setcounter{footnote}{0}

  \if@noheadings
  \else
    \thispagestyle{empty}
%     \markboth{\@Shorttitle}{\@Hyperauthor}
%     \pagestyle{myheadings}
\pagestyle{fancy}\fancyhf{}
\fancyhf{}
\fancyhead[LE]{\small\thepage}
\fancyhead[LO]{\small\@Hyperauthor~(\@Year)}
\fancyhead[RE]{\small\@Shorttitle}
\fancyhead[RO]{\small\thepage}
\fancyfoot[LE]{\iclogo}
\fancyfoot[RO]{\iclogo}
  \fi

  \let\maketitle\relax \let\@maketitle\relax
  \gdef\@thanks{}\gdef\@author{}\gdef\@title{}\let\thanks\relax
}

\def\@maketitle{\vbox{\hsize\textwidth \linewidth\hsize
\if@nojss
  %% \vskip 1in
\else
  \vskip .5in
\fi
  {\centering
  {\LARGE\bf \@title\par}
  \vskip 0.2in plus 1fil minus 0.1in
  {
      \def\and{\unskip\enspace{\rm and}\enspace}%
      \def\And{\end{tabular}\hss \egroup \hskip 1in plus 2fil
	\hbox to 0pt\bgroup\hss \begin{tabular}[t]{c}\large\bf\rule{\z@}{24pt}\ignorespaces}%
      \def\AND{\end{tabular}\hss\egroup \hfil\hfil\egroup
	\vskip 0.1in plus 1fil minus 0.05in
	\hbox to \linewidth\bgroup\rule{\z@}{10pt} \hfil\hfil
	\hbox to 0pt\bgroup\hss \begin{tabular}[t]{c}\large\bf\rule{\z@}{24pt}\ignorespaces}
      \hbox to \linewidth\bgroup\rule{\z@}{10pt} \hfil\hfil
      \hbox to 0pt\bgroup\hss \begin{tabular}[t]{c}\large\bf\rule{\z@}{24pt}\@author
      \end{tabular}\hss\egroup
  \hfil\hfil\egroup}
  \vskip 0.3in minus 0.1in
  \hrule
  \begin{abstract}
  \@Abstract
  \end{abstract}}
  \textit{Palavras-chave}:~\@Keywords.
  \vskip 0.1in minus 0.05in
  \hrule
  \vskip 0.2in minus 0.1in
}}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  

%% colors
\definecolor{Red}{rgb}{0.5,0,0}
\definecolor{Blue}{rgb}{0,0,0.5}


  \hypersetup{%
%     hyperindex = {true},
    colorlinks = {true},
    linktocpage = {true},
    plainpages = {false},
    linkcolor = {Blue},
    citecolor = {Blue},
    urlcolor = {Red},
    pdfstartview = {Fit},
    pdfpagemode = {UseOutlines},
    pdfview = {XYZ null null null}
  }


  
  \AtBeginDocument{
    \hypersetup{%
      pdfauthor = {\@Hyperauthor},
      pdftitle = {\@Plaintitle},
      pdfsubject = {\@Hypersubject},
      pdfkeywords = {\@Plainkeywords}
    }
  }

  \AtBeginDocument{\maketitle}

%% commands
\newcommand\code{\bgroup\@makeother\_\@makeother\~\@makeother\$\@codex}
\def\@codex#1{{\normalfont\ttfamily\hyphenchar\font=-1 #1}\egroup}
%%\let\code=\texttt
\let\proglang=\textsf
\newcommand{\pkg}[1]{{\fontseries{b}\selectfont #1}}
\newcommand{\email}[1]{\href{mailto:#1}{\normalfont\texttt{#1}}}
\ifx\csname urlstyle\endcsname\relax
  \newcommand\@doi[1]{doi:\discretionary{}{}{}#1}\else
  \newcommand\@doi{doi:\discretionary{}{}{}\begingroup
\urlstyle{tt}\Url}\fi
\newcommand{\doi}[1]{\href{http://dx.doi.org/#1}{\normalfont\texttt{\@doi{#1}}}}
\newcommand{\E}{\mathsf{E}}
\newcommand{\VAR}{\mathsf{VAR}}
\newcommand{\COV}{\mathsf{COV}}
\newcommand{\Prob}{\mathsf{P}}
\endinput
%%
%% End of file `jss.cls'.
