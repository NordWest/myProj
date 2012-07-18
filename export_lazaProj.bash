#!/bin/bash

lpDir="./../lazaProj/"

svn export --force astro $lpDir/astro
svn export --force libs $lpDir/libs
svn export --force mb $lpDir/mb
svn export --force cdsfind $lpDir/cdsfind
svn export --force mpeph $lpDir/mpeph
svn export --force fitsWCS $lpDir/fitsWCS

